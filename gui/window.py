import ctypes
import re
import subprocess
import sys
from pathlib import Path
from PySide6.QtWidgets import QTextEdit
from PySide6.QtGui import QTextCursor
from PySide6.QtCore import Qt, QTimer, Signal
from PySide6.QtGui import QColor, QFont, QTextCharFormat, QSyntaxHighlighter, QTextFormat, QPainter
from PySide6.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QSplitter, QPlainTextEdit,
    QFileDialog, QMessageBox, QPushButton, QLabel, QTableWidget, QTableWidgetItem,
    QTabWidget, QGroupBox, QComboBox, QSpinBox, QLineEdit, QHeaderView,
    QStatusBar, QToolBar, QAbstractItemView
)

GUI_DIR = Path(__file__).resolve().parent
ROOT = GUI_DIR.parent
BUILD = GUI_DIR / "build"
ASM_EXE = BUILD / "sac16_asm"
LIB_PATH = BUILD / "libsac16.so"

class AsmHighlighter(QSyntaxHighlighter):
    def __init__(self, document):
        super().__init__(document)
        self.rules = []
        def rule(pattern, color, bold=False):
            fmt = QTextCharFormat()
            fmt.setForeground(QColor(color))
            if bold:
                fmt.setFontWeight(QFont.Bold)
            self.rules.append((re.compile(pattern, re.IGNORECASE), fmt))
        rule(r"\\b(LD|ST|MOV|ADD|SUB|MUL|AND|OR|NOT|JMP|JZ|JNZ|CALL|RET|LDI|HALT)\\b", "#79c0ff", True)
        rule(r"\\b(R[0-7]|ACC)\\b", "#d2a8ff", True)
        rule(r"\\b(\\.text|\\.data|\\.word)\\b", "#7ee787", True)
        rule(r"\\b[A-Za-z_][A-Za-z0-9_]*(?=\\s*:)", "#ffa657", True)
        rule(r";.*$", "#8b949e")
        rule(r"\\b(0x[0-9a-fA-F]+|\\d+)\\b", "#a5d6ff")
    def highlightBlock(self, text):
        for pattern, fmt in self.rules:
            for match in pattern.finditer(text):
                self.setFormat(match.start(), match.end() - match.start(), fmt)

class LineNumberArea(QWidget):
    def __init__(self, editor):
        super().__init__(editor)
        self.editor = editor
    def sizeHint(self):
        return self.editor.line_number_area_width(), 0
    def paintEvent(self, event):
        self.editor.paint_line_number_area(event)
    def mousePressEvent(self, event):
        self.editor.toggle_breakpoint_at(event.position().y())

class AssemblyEditor(QPlainTextEdit):
    breakpointToggled = Signal(int, bool)
    def __init__(self, parent=None):
        super().__init__(parent)
        self.breakpoints = set()
        self.current_line = None
        self.line_area = LineNumberArea(self)
        self.blockCountChanged.connect(self.update_line_number_area_width)
        self.updateRequest.connect(self.update_line_number_area)
        self.cursorPositionChanged.connect(self.highlight_current_line)
        self.update_line_number_area_width(0)
        self.setLineWrapMode(QPlainTextEdit.NoWrap)
        self.setFont(QFont("Monospace", 11))
        self.highlighter = AsmHighlighter(self.document())
        self.setStyleSheet("QPlainTextEdit { background:#0d1117; color:#e6edf3; border:0; padding:6px; }")
    def line_number_area_width(self):
        digits = max(3, len(str(max(1, self.blockCount()))))
        return 18 + self.fontMetrics().horizontalAdvance("9") * digits
    def update_line_number_area_width(self, _):
        self.setViewportMargins(self.line_number_area_width(), 0, 0, 0)
    def update_line_number_area(self, rect, dy):
        if dy:
            self.line_area.scroll(0, dy)
        else:
            self.line_area.update(0, rect.y(), self.line_area.width(), rect.height())
        if rect.contains(self.viewport().rect()):
            self.update_line_number_area_width(0)
    def resizeEvent(self, event):
        super().resizeEvent(event)
        cr = self.contentsRect()
        self.line_area.setGeometry(cr.left(), cr.top(), self.line_number_area_width(), cr.height())
    def paint_line_number_area(self, event):
        painter = QPainter(self.line_area)
        painter.fillRect(event.rect(), QColor("#161b22"))
        block = self.firstVisibleBlock()
        number = block.blockNumber()
        top = int(self.blockBoundingGeometry(block).translated(self.contentOffset()).top())
        bottom = top + int(self.blockBoundingRect(block).height())
        while block.isValid() and top <= event.rect().bottom():
            if block.isVisible() and bottom >= event.rect().top():
                line = number + 1
                if line in self.breakpoints:
                    painter.setBrush(QColor("#f85149"))
                    painter.setPen(Qt.NoPen)
                    painter.drawEllipse(3, top + 3, 9, 9)
                painter.setPen(QColor("#8b949e"))
                painter.drawText(self.line_area.width() - 7 - self.fontMetrics().horizontalAdvance(str(line)),
                                 top, self.fontMetrics().horizontalAdvance(str(line)) + 2,
                                 self.fontMetrics().height(), Qt.AlignRight, str(line))
            block = block.next()
            top = bottom
            bottom = top + int(self.blockBoundingRect(block).height())
            number += 1
    def toggle_breakpoint_at(self, y):
        block = self.firstVisibleBlock()
        top = int(self.blockBoundingGeometry(block).translated(self.contentOffset()).top())
        bottom = top + int(self.blockBoundingRect(block).height())
        while block.isValid():
            if block.isVisible() and top <= y <= bottom:
                line = block.blockNumber() + 1
                enabled = line not in self.breakpoints
                if enabled: self.breakpoints.add(line)
                else: self.breakpoints.remove(line)
                self.line_area.update()
                self.breakpointToggled.emit(line, enabled)
                return
            block = block.next()
            top = bottom
            bottom = top + int(self.blockBoundingRect(block).height())
    def highlight_current_line(self):
        selections = []
        if self.current_line is not None:
            block = self.document().findBlockByNumber(self.current_line - 1)

            if block.isValid():
                sel = QTextEdit.ExtraSelection()

                sel.cursor = self.textCursor()
                sel.cursor.setPosition(block.position())
                sel.cursor.movePosition(
                    QTextCursor.EndOfBlock,
                    QTextCursor.KeepAnchor
                )

                sel.format.setBackground(QColor("#263b2d"))
                sel.format.setProperty(QTextFormat.FullWidthSelection, True)

                selections.append(sel)

        self.setExtraSelections(selections)
    def set_current_line(self, line):
        self.current_line = line
        if line is not None:
            block = self.document().findBlockByNumber(line - 1)
            if block.isValid():
                cursor = self.textCursor()
                cursor.setPosition(block.position())
                self.setTextCursor(cursor)
                self.centerCursor()
        self.highlight_current_line()

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("SAC16 Studio")
        self.resize(1450, 900)
        self.asm_path = None
        self.cpu = None
        self.lib = None
        self.source_to_address = {}
        self.address_to_line = {}
        self.breakpoints = set()
        self.watchpoints = []
        self.stats = {}
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.run_tick)
        self.running = False
        self.paused_breakpoint = None
        self.skip_breakpoint_once = None
        self._build_ui()
        self._load_backend()
        self._reset_stats()
    def _build_ui(self):
        toolbar = QToolBar("Main")
        toolbar.setMovable(False)
        self.addToolBar(toolbar)
        for label, callback in [
            ("Open", self.open_file), ("Save", self.save_file), ("Build", self.build_program),
            ("Run", self.run_program), ("Step", self.step_once), ("Continue", self.continue_run),
            ("Stop", self.stop_run), ("Reset", self.reset_cpu)
        ]:
            button = QPushButton(label)
            button.clicked.connect(callback)
            toolbar.addWidget(button)
        toolbar.addSeparator()
        toolbar.addWidget(QLabel("Speed:"))
        self.speed = QComboBox()
        self.speed.addItems(["1 step/s", "5 steps/s", "20 steps/s", "Fast"])
        self.speed.setCurrentIndex(2)
        toolbar.addWidget(self.speed)

        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)
        main_split = QSplitter(Qt.Horizontal)
        self.editor = AssemblyEditor()
        self.editor.setPlaceholderText("Open or write a SAC16 assembly file...")
        self.editor.breakpointToggled.connect(self.on_gutter_breakpoint)
        main_split.addWidget(self.editor)

        right_split = QSplitter(Qt.Vertical)
        self.register_table = self._table(["Register", "Hex", "Decimal"])
        right_split.addWidget(self._group("CPU Registers", self.register_table))

        mem_widget = QWidget()
        mem_layout = QVBoxLayout(mem_widget)
        mem_controls = QHBoxLayout()
        self.mem_address = QLineEdit("0x0000")
        self.mem_address.setMaximumWidth(120)
        goto = QPushButton("Go to address")
        goto.clicked.connect(self.goto_memory)
        mem_controls.addWidget(QLabel("Address:"))
        mem_controls.addWidget(self.mem_address)
        mem_controls.addWidget(goto)
        mem_controls.addStretch()
        mem_layout.addLayout(mem_controls)
        self.memory_table = self._table(["Address", "Hex", "Decimal"])
        mem_layout.addWidget(self.memory_table)
        right_split.addWidget(self._group("Data Memory", mem_widget))
        main_split.addWidget(right_split)
        main_split.setSizes([850, 500])
        layout.addWidget(main_split, 5)

        tabs = QTabWidget()
        self.imem_table = self._table(["Address", "Machine Code", "Instruction", "Source Line"])
        tabs.addTab(self.imem_table, "Instruction Memory")
        self.console = QPlainTextEdit()
        self.console.setReadOnly(True)
        self.console.setMaximumBlockCount(3000)
        self.console.setStyleSheet("background:#0d1117;color:#c9d1d9;font-family:monospace;")
        tabs.addTab(self.console, "Debug Console")
        self.stats_view = QPlainTextEdit()
        self.stats_view.setReadOnly(True)
        tabs.addTab(self.stats_view, "Execution Statistics")
        bp_panel = QWidget()
        bp_layout = QHBoxLayout(bp_panel)
        self.bp_line = QSpinBox()
        self.bp_line.setMinimum(1)
        self.bp_line.setMaximum(100000)
        bp_add = QPushButton("Toggle Breakpoint at Source Line")
        bp_add.clicked.connect(self.toggle_breakpoint_line)
        self.bp_list = QPlainTextEdit()
        self.bp_list.setReadOnly(True)
        bp_layout.addWidget(QLabel("Source line:"))
        bp_layout.addWidget(self.bp_line)
        bp_layout.addWidget(bp_add)
        bp_layout.addWidget(self.bp_list, 1)
        tabs.addTab(bp_panel, "Breakpoints")
        watch_panel = QWidget()
        watch_layout = QHBoxLayout(watch_panel)
        self.watch_type = QComboBox()
        self.watch_type.addItems(["ACC", "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "Memory"])
        self.watch_address = QLineEdit("0x0000")
        self.watch_address.setMaximumWidth(120)
        add_watch = QPushButton("Add Watchpoint")
        add_watch.clicked.connect(self.add_watchpoint)
        self.watch_list = QPlainTextEdit()
        self.watch_list.setReadOnly(True)
        watch_layout.addWidget(self.watch_type)
        watch_layout.addWidget(self.watch_address)
        watch_layout.addWidget(add_watch)
        watch_layout.addWidget(self.watch_list, 1)
        tabs.addTab(watch_panel, "Watchpoints")
        layout.addWidget(tabs, 3)
        self.setStatusBar(QStatusBar())
        self.statusBar().showMessage("Ready")
        self.editor.setPlainText(".text\\n\\nHALT\\n")
    def _group(self, title, widget):
        group = QGroupBox(title)
        box = QVBoxLayout(group)
        box.addWidget(widget)
        return group
    def _table(self, headers):
        table = QTableWidget(0, len(headers))
        table.setHorizontalHeaderLabels(headers)
        table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        table.setEditTriggers(QAbstractItemView.NoEditTriggers)
        table.setSelectionBehavior(QAbstractItemView.SelectRows)
        table.setAlternatingRowColors(True)
        return table
    def _log(self, text):
        self.console.appendPlainText(str(text))
    def _load_backend(self):
        self._add_build_menu()
        if not ASM_EXE.exists() or not LIB_PATH.exists():
            self._log("C backend is not built. Select Tools > Build C backend.")
            return
        try:
            self.lib = ctypes.CDLL(str(LIB_PATH))
            self._configure_api()
            self.cpu = self.lib.sac_create()
            if not self.cpu: raise RuntimeError("Could not allocate CPU state.")
            self._log("C simulator backend loaded.")
        except Exception as exc:
            QMessageBox.critical(self, "Backend error", str(exc))
    def _add_build_menu(self):
        menu = self.menuBar().addMenu("Tools")
        act = menu.addAction("Build C backend")
        act.triggered.connect(self.build_backend)
    def _configure_api(self):
        L = self.lib
        L.sac_create.restype = ctypes.c_void_p
        L.sac_destroy.argtypes = [ctypes.c_void_p]
        L.sac_load.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
        L.sac_load.restype = ctypes.c_int
        L.sac_step.argtypes = [ctypes.c_void_p]
        L.sac_step.restype = ctypes.c_int
        L.sac_reset.argtypes = [ctypes.c_void_p]
        for n in ["sac_pc","sac_sp","sac_acc","sac_reg","sac_zero","sac_carry","sac_halted",
                  "sac_error","sac_imem_count","sac_dmem_count","sac_imem_at","sac_dmem_at"]:
            getattr(L, n).argtypes = [ctypes.c_void_p] if n not in ("sac_reg","sac_imem_at","sac_dmem_at") else (
                [ctypes.c_void_p, ctypes.c_int] if n == "sac_reg" else [ctypes.c_void_p, ctypes.c_size_t])
        for n in ["sac_pc","sac_sp","sac_acc","sac_reg","sac_imem_at","sac_dmem_at"]:
            getattr(L,n).restype = ctypes.c_uint16
        for n in ["sac_zero","sac_carry","sac_halted","sac_error"]:
            getattr(L,n).restype = ctypes.c_int
        for n in ["sac_imem_count","sac_dmem_count"]:
            getattr(L,n).restype = ctypes.c_size_t
        L.sac_error_text.argtypes = [ctypes.c_void_p]
        L.sac_error_text.restype = ctypes.c_char_p
        L.sac_disassemble.argtypes = [ctypes.c_uint16, ctypes.c_char_p, ctypes.c_size_t]
        L.sac_disassemble.restype = ctypes.c_int
    def build_backend(self):
        try:
            subprocess.run([sys.executable, str(GUI_DIR / "build_backend.py")], check=True)
            QMessageBox.information(self, "Build complete", "C backend built. Restart SAC16 Studio to load it.")
        except subprocess.CalledProcessError as exc:
            QMessageBox.critical(self, "Build failed", str(exc))
    def open_file(self):
        path, _ = QFileDialog.getOpenFileName(self, "Open SAC16 Assembly", str(ROOT), "Assembly (*.asm);;All files (*)")
        if not path: return
        self.asm_path = Path(path)
        self.editor.setPlainText(self.asm_path.read_text(encoding="utf-8"))
        self.breakpoints.clear()
        self.editor.breakpoints.clear()
        self.editor.line_area.update()
        self.breakpoints.clear()
        self.source_to_address, self.address_to_line = self.make_source_map(self.editor.toPlainText())
        self._update_breakpoint_list()
        self._log(f"Opened {self.asm_path}")
    def save_file(self):
        if self.asm_path is None:
            path, _ = QFileDialog.getSaveFileName(self, "Save Assembly", str(ROOT / "program.asm"), "Assembly (*.asm)")
            if not path: return False
            self.asm_path = Path(path)
        self.asm_path.write_text(self.editor.toPlainText(), encoding="utf-8")
        self._log(f"Saved {self.asm_path}")
        return True
    @staticmethod
    def make_source_map(source):
        line_to_addr, addr_to_line = {}, {}
        section, address = None, 0
        for line_no, raw in enumerate(source.splitlines(), 1):
            line = raw.split(";", 1)[0].strip()
            if not line: continue
            if line.lower() == ".text":
                section = "text"; continue
            if line.lower() == ".data":
                section = "data"; continue
            if section != "text": continue
            while ":" in line:
                before, after = line.split(":", 1)
                if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", before.strip()): break
                line = after.strip()
                if not line: break
            if not line or line.startswith("."): continue
            line_to_addr[line_no] = address
            addr_to_line[address] = line_no
            address += 1
        return line_to_addr, addr_to_line
    def on_gutter_breakpoint(self, line, enabled):
        self.source_to_address, self.address_to_line = self.make_source_map(self.editor.toPlainText())
        address = self.source_to_address.get(line)
        if address is None:
            self.editor.breakpoints.discard(line)
            self.editor.line_area.update()
            QMessageBox.warning(self, "Breakpoint", "This source line is not an instruction in the .text section.")
            return
        if enabled: self.breakpoints.add(address)
        else: self.breakpoints.discard(address)
        self._update_breakpoint_list()
    def toggle_breakpoint_line(self):
        line = self.bp_line.value()
        enabled = line not in self.editor.breakpoints
        if enabled: self.editor.breakpoints.add(line)
        else: self.editor.breakpoints.remove(line)
        self.editor.line_area.update()
        self.on_gutter_breakpoint(line, enabled)
    def _update_breakpoint_list(self):
        self.bp_list.setPlainText("\\n".join(
            f"Line {self.address_to_line.get(a, '?')} -> address 0x{a:04X}"
            for a in sorted(self.breakpoints)))
    def build_program(self):
        if self.asm_path is None:
            if not self.save_file(): return
        else:
            if not self.save_file(): return
        if not self.lib or not ASM_EXE.exists():
            QMessageBox.warning(self, "Backend missing", "Build the C backend first: Tools > Build C backend.")
            return
        self.source_to_address, self.address_to_line = self.make_source_map(self.editor.toPlainText())
        work = BUILD / "program"
        work.mkdir(parents=True, exist_ok=True)
        asm_copy = work / "program.asm"
        asm_copy.write_text(self.editor.toPlainText(), encoding="utf-8")
        try:
            result = subprocess.run([str(ASM_EXE), str(asm_copy), "-o", str(work / "program.asm")],
                                    cwd=str(work), capture_output=True, text=True)
            if result.stdout: self._log(result.stdout)
            if result.stderr: self._log(result.stderr)
            if result.returncode != 0:
                QMessageBox.critical(self, "Assembly failed", result.stdout + result.stderr); return
            ok = self.lib.sac_load(self.cpu, str(work / "program.bin").encode(), str(work / "program.data").encode())
            if not ok:
                QMessageBox.critical(self, "Load failed", "C simulator could not load generated binary/data."); return
            count = int(self.lib.sac_imem_count(self.cpu))
            # Source-map and assembler output must agree; otherwise refuse source breakpoints.
            if count != len(self.address_to_line):
                self._log(f"[WARNING] Source map has {len(self.address_to_line)} instructions; assembler emitted {count}.")
            self.breakpoints = {a for a in self.breakpoints if a < count}
            self.watchpoints.clear()
            self.watch_list.clear()
            self._reset_stats()
            self._refresh_all()
            self._update_breakpoint_list()
            self._log(f"[OK] Build successful: {count} instructions.")
        except Exception as exc:
            QMessageBox.critical(self, "Build error", str(exc))
    def _reset_stats(self):
        self.stats = dict(instructions=0, cycles=0, memory=0, stack=0, branches=0, taken=0, not_taken=0)
    def _watch_value(self, spec):
        kind, value = spec
        if kind == "reg": return int(self.lib.sac_acc(self.cpu) if value == 8 else self.lib.sac_reg(self.cpu, value))
        return int(self.lib.sac_dmem_at(self.cpu, value))
    def add_watchpoint(self):
        if not self.cpu: return
        kind = self.watch_type.currentText()
        if kind == "Memory":
            try:
                address = int(self.watch_address.text(), 0)
                if not 0 <= address <= 65535: raise ValueError
            except ValueError:
                QMessageBox.warning(self, "Invalid address", "Enter an address from 0 to 65535."); return
            spec, label = ("mem", address), f"Memory[0x{address:04X}]"
        else:
            idx = 8 if kind == "ACC" else int(kind[1:])
            spec, label = ("reg", idx), kind
        if spec not in self.watchpoints:
            self.watchpoints.append(spec)
            self.watch_list.appendPlainText(label)
            self._log(f"Watchpoint added: {label}")
    def _check_watchpoints(self, before):
        hits = []
        for spec in self.watchpoints:
            old, new = before[spec], self._watch_value(spec)
            if old != new:
                kind, value = spec
                name = ("ACC" if value == 8 else f"R{value}") if kind == "reg" else f"Memory[0x{value:04X}]"
                hits.append(f"{name}: 0x{old:04X} -> 0x{new:04X}")
        if hits:
            self.stop_run()
            for hit in hits: self._log("[WATCHPOINT] " + hit)
            self.statusBar().showMessage("Paused: Watchpoint hit")
            self._refresh_all()
            return True
        return False
    def _execute_step(self):
        if not self.cpu: return False
        if self.lib.sac_halted(self.cpu):
            self.stop_run(); self.statusBar().showMessage("Program halted"); return False
        pc = int(self.lib.sac_pc(self.cpu))
        count = int(self.lib.sac_imem_count(self.cpu))
        if pc >= count:
            self.stop_run()
            self._log(f"[CPU ERROR] PC 0x{pc:04X} is outside loaded instruction range.")
            self.statusBar().showMessage("Program counter out of loaded range")
            return False
        if pc in self.breakpoints and self.skip_breakpoint_once != pc:
            self.stop_run()
            self.paused_breakpoint = pc
            self._log(f"[BREAKPOINT] Paused before instruction at 0x{pc:04X}")
            self._refresh_all()
            return False
        if self.skip_breakpoint_once == pc:
            self.skip_breakpoint_once = None
        self.paused_breakpoint = None
        before = {spec: self._watch_value(spec) for spec in self.watchpoints}
        instruction = int(self.lib.sac_imem_at(self.cpu, pc))
        opcode = (instruction >> 12) & 0xF
        previous_zero = bool(self.lib.sac_zero(self.cpu))
        if not self.lib.sac_step(self.cpu):
            self.stop_run()
            msg = self.lib.sac_error_text(self.cpu).decode(errors="replace")
            self._log("[CPU ERROR] " + msg)
            QMessageBox.critical(self, "CPU error", msg)
            self._refresh_all(); return False
        self.stats["instructions"] += 1
        self.stats["cycles"] += 2 if opcode in (0, 1, 12, 13) else (3 if opcode == 5 else 1)
        if opcode in (0, 1): self.stats["memory"] += 1
        if opcode in (12, 13): self.stats["stack"] += 1
        if opcode in (10, 11):
            self.stats["branches"] += 1
            taken = previous_zero if opcode == 10 else not previous_zero
            self.stats["taken" if taken else "not_taken"] += 1
        self._refresh_all()
        if self._check_watchpoints(before): return False
        if self.lib.sac_halted(self.cpu):
            self.stop_run(); self._log("[INFO] HALT reached.")
            self.statusBar().showMessage("Program halted"); return False
        return True
    def step_once(self):
        self.stop_run()
        pc = int(self.lib.sac_pc(self.cpu)) if self.cpu else None
        if pc is not None and pc in self.breakpoints:
            self.skip_breakpoint_once = pc
        self._execute_step()
    def run_program(self):
        if not self.cpu: return
        if int(self.lib.sac_imem_count(self.cpu)) == 0:
            self.build_program()
            if int(self.lib.sac_imem_count(self.cpu)) == 0: return
        self._reset_stats()
        self.continue_run()
    def continue_run(self):
        if not self.cpu or self.running: return
        if self.paused_breakpoint is not None:
            self.skip_breakpoint_once = self.paused_breakpoint
        self.running = True
        self.timer.start([1000, 200, 50, 1][self.speed.currentIndex()])
        self.statusBar().showMessage("Running")
    def run_tick(self):
        if not self.running: return
        batch = 100 if self.speed.currentIndex() == 3 else 1
        for _ in range(batch):
            if not self.running: break
            if not self._execute_step(): break
    def stop_run(self):
        self.running = False
        self.timer.stop()
    def reset_cpu(self):
        self.stop_run()
        if self.cpu:
            self.lib.sac_reset(self.cpu)
            self._reset_stats()
            self._refresh_all()
            self._log("[INFO] CPU reset.")
    def goto_memory(self):
        if not self.cpu: return
        try:
            address = int(self.mem_address.text(), 0)
            if not 0 <= address <= 65535: raise ValueError
        except ValueError:
            QMessageBox.warning(self, "Invalid address", "Enter an address from 0 to 65535."); return
        self.memory_table.setRowCount(1)
        self._set_row(self.memory_table, 0, [f"0x{address:04X}", f"0x{self.lib.sac_dmem_at(self.cpu,address):04X}", str(self.lib.sac_dmem_at(self.cpu,address))])
    def _set_row(self, table, row, values):
        for col, value in enumerate(values):
            item = QTableWidgetItem(str(value))
            item.setTextAlignment(Qt.AlignCenter)
            table.setItem(row, col, item)
    def _refresh_all(self):
        if not self.cpu: return
        values = [("PC", self.lib.sac_pc(self.cpu)), ("SP", self.lib.sac_sp(self.cpu)), ("ACC", self.lib.sac_acc(self.cpu))]
        values += [(f"R{i}", self.lib.sac_reg(self.cpu, i)) for i in range(8)]
        self.register_table.setRowCount(len(values) + 2)
        for row, (name, value) in enumerate(values):
            self._set_row(self.register_table, row, [name, f"0x{value:04X}", str(value)])
        self._set_row(self.register_table, len(values), ["Zero", str(self.lib.sac_zero(self.cpu)), ""])
        self._set_row(self.register_table, len(values)+1, ["Carry", str(self.lib.sac_carry(self.cpu)), ""])
        dcount = min(int(self.lib.sac_dmem_count(self.cpu)), 256)
        self.memory_table.setRowCount(dcount)
        for i in range(dcount):
            value = int(self.lib.sac_dmem_at(self.cpu, i))
            self._set_row(self.memory_table, i, [f"0x{i:04X}", f"0x{value:04X}", str(value)])
        icount = int(self.lib.sac_imem_count(self.cpu))
        self.imem_table.setRowCount(icount)
        for i in range(icount):
            word = int(self.lib.sac_imem_at(self.cpu, i))
            buf = ctypes.create_string_buffer(128)
            self.lib.sac_disassemble(word, buf, len(buf))
            self._set_row(self.imem_table, i, [f"0x{i:04X}", f"0x{word:04X}", buf.value.decode(errors="replace"), str(self.address_to_line.get(i, ""))])
        pc = int(self.lib.sac_pc(self.cpu))
        line = self.address_to_line.get(pc)
        self.editor.set_current_line(line)
        self.stats_view.setPlainText(
            f"Instructions: {self.stats['instructions']}\\nCycles: {self.stats['cycles']}\\n"
            f"CPI: {self.stats['cycles']/self.stats['instructions'] if self.stats['instructions'] else 0:.2f}\\n"
            f"Memory operations: {self.stats['memory']}\\nStack operations: {self.stats['stack']}\\n"
            f"Conditional branches: {self.stats['branches']}\\nTaken: {self.stats['taken']}\\n"
            f"Not taken: {self.stats['not_taken']}\\nCPU: {'Halted' if self.lib.sac_halted(self.cpu) else 'Ready'}"
        )
        self.statusBar().showMessage(f"PC = 0x{pc:04X}")
    def closeEvent(self, event):
        self.stop_run()
        if self.cpu and self.lib:
            self.lib.sac_destroy.argtypes = [ctypes.c_void_p]
            self.lib.sac_destroy(self.cpu)
        event.accept()
