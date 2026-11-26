# Change this to the exact FPGA part on your board.
set part_name "xc7a35tcpg236-1" ;# Example only: Basys 3
set root_dir [file normalize [file dirname [info script]]/..]
file mkdir [file join $root_dir build]
create_project sac16_vivado [file join $root_dir vivado_project] -part $part_name -force
add_files [list [file join $root_dir rtl sac16_alu.v] [file join $root_dir rtl sac16_regfile.v] [file join $root_dir rtl sac16_imem.v] [file join $root_dir rtl sac16_dmem.v] [file join $root_dir rtl sac16_cpu.v]]
set_property top sac16_cpu [current_fileset]
update_compile_order -fileset sources_1
add_files -norecurse [list [file join $root_dir program.hex] [file join $root_dir data.hex]]
set_property file_type {Memory Initialization Files} [get_files *.hex]
launch_runs synth_1 -jobs 4
wait_on_run synth_1
open_run synth_1
report_utilization -file [file join $root_dir build vivado_utilization.rpt]
report_timing_summary -file [file join $root_dir build vivado_timing.rpt]
