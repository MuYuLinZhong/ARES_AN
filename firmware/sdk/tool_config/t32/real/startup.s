; ============================================================================
; Copyright (c) 2021 Infineon Technologies AG
;               All rights reserved.
;               www.infineon.com
; ============================================================================
;
; ============================================================================
; Redistribution and use of this software only permitted to the extent
; expressly agreed with Infineon Technologies AG.
; ============================================================================

; show window for printouts
area

print "Entering'"+os.ppf()+"'"

; setup a variable containing this script name (e.g. used by the reset button)
local &STARTUP_SCRIPT
local &reset_handler
&reset_handler=P:0x0

&STARTUP_SCRIPT=os.ppf()

; fetch script parameters
entry &rom_elf_filename &nvm_elf_filename &window_pos_script &window_menu_script &user_setup_script &per_file &nvm_binary &do_not_load_menu_again &dparam_elf_filename

print "-- Preparing T32 real setup"

print "-- Config CPU: CortexM0"
system.cpu.CORTEXM0 
system.cpuaccess ENABLE
print "-- Config: SWD, DAP, DUALPORT"
system.config.debugporttype.SWD 
system.MEMACCESS DAP 
system.option.dualport.ON 
system.option.RESBREAK ON
system.option.ENRESET ON
system.option.TRST.OFF 
SYStem.Option PWRDWNRecover OFF
system.option.WaitReset 1ms

; Dont stop at reset vector upon core reset
TrOnChip.Set CORERESET OFF
; Dont stop at hard fault vector upon hard fault
TrOnchip.Set HARDERR OFF

; for MikroProg isolation adapter we need to reduce the speed from 10MHz (default) to 1MHz
;system.JtagClock 26MHz
system.JtagClock 10MHz

print "-- Attach Debugger"
sys.attach
print "-- Break (Stop) CPU"
break

; Map ROM/Flash Area for single hardware breakpoint
Map.BOnchip 0x0--0x1ffff

; remove all breakpoints from previous session
Break.Delete

; Load the NVM binary here.
if file.exist(&nvm_elf_filename)
(
    print "-- Loading &nvm_elf_filename (code space only!)"
    FLASH.RESet
    DIALOG.YESNO "(Re)Program NVM memory?"
    LOCAL &progflash
    ENTRY &progflash
    IF &progflash 
    (
        ; --------------------------------------------------------------------------------
        ; Flash declaration
        IF CPUIS(CortexM0)
            FLASH.Create 1. 00010000--000103FF 0x80 TARGET Long
            FLASH.Create 2. 00010400--000107FF 0x80 TARGET Long
            FLASH.Create 3. 00010800--0001EFFF 0x80 TARGET Long
        ELSE
        (
            PRINT %ERROR "FLASH size of CPU type is not supported by the script"
            ENDDO
        )
        FLASH.TARGET 0x00021300 0x00021000 0x100 &nvm_binary

        FLASH.Erase 2.
        Flash.Program 2.
        Data.LOAD.ELF &nvm_elf_filename 0x10400--0x107ff /verify

        FLASH.Erase 3.
        Flash.Program 3.
        Data.LOAD.ELF &nvm_elf_filename 0x10800--0x1efff /noclear /verify

        // load DPARAM after NVM image (e.g. after APARAM) to make sure that "/noclear" is omitted on the first executed LOAD statement
        if file.exist(&dparam_elf_filename)
	    (
            print "-- Loading disabled: &dparam_elf_filename (dparam)"
            ; Do not load DAPRAM, keep chip defaults. Writing of default DAPRAMs will overwrite chip trimming.
            ;FLASH.Erase 1.
            ;Flash.Program 1.
            ;Data.LOAD.ELF &dparam_elf_filename
        )
        else
        (
            print "-- Skipping dparam"
        ) 

        FLASH.Program off
    )
    ELSE
    (
        Data.LOAD.ELF &nvm_elf_filename 0x10400--0x1efff /nocode /register /noclear /verify
    )

    Break.Set _nvm_start
)


; Load the ROM binary here.
if file.exist(&rom_elf_filename)
(
    print "-- Loading &rom_elf_filename"
    ; Data.LOAD &rom_elf_filename /nocode /noregister /noclear /spath 
    Data.LOAD &rom_elf_filename 0x0000--0x3ff7 /nocode /noclear /VERIFY

    ; use reset handler from ROM image as entry point
    ; We allow for two distict names:
    ; - smack_lib code (image_rom) uses "Reset_Handler".
    ; - verification projects use "main".
    if (symbol.exist(Reset_Handler))
    (
        print "-- Using symbol 'Reset_Handler' as reset vector."
        &reset_handler=var.address(Reset_Handler)
    )
    else if (symbol.exist(main))
    (
        print "-- Using symbol 'main' as reset vector."
        &reset_handler=var.address(main)
    )
    else if (symbol.exist(NVM_Reset_Handler))
    (
		; fallback to NVM if none of the ROM symbols was found
        print "-- Using symbol 'NVM_Reset_Handler' as reset vector."
        &reset_handler=var.address(NVM_Reset_Handler)
    )
    else if (Data.Word(p:4)>0&&Data.Word(p:4)<0x1f000)
    (
        print "-- Using word at address 0x0004 as reset vector."
        &reset_handler=P:Data.Word(p:4)-1
        print "-- val: &reset_handler"
    )
    else
    (
        print %error "-- Neither symbol 'Reset_Handler' nor 'main' found. Terminating..."
    )

    ; use __StackTop from ROM image as init value of the stack pointer
    if (symbol.exist(__StackTop))
    (
        print "-- Using symbol '__StackTop' as stack pointer."
        r.s MSP __StackTop
    )
    else
    (
        print %error "-- Symbol '__initial_sp' not found. Terminating..."
    )
)


if (&reset_handler==P:0x0)
(
    print %error "ERROR: No ROM loaded or reset vector not found."
    enddo
)

print "window_menu_script &window_menu_script &do_not_load_menu_again"
if (&do_not_load_menu_again==0)
(
    if file.exist(&window_menu_script)
    (
        print "-- Setup windows menu"
        &do_not_load_menu_again=1
        do &window_menu_script &STARTUP_SCRIPT &rom_elf_filename &nvm_elf_filename &window_pos_script &user_setup_script &per_file &nvm_binary &do_not_load_menu_again &dparam_elf_filename
        &do_not_load_menu_again=0
    )
)

print "window_pos_script"
if file.exist(&window_pos_script)
(
    print "-- Setup windows"
    do &window_pos_script &per_file
)
else
(
    print "-- Setup default windows "
    ; open code window
    list

    ; open register window
    register

    ; open peripheral window
    per &per_file
)

print "-- Looking for user setup at &user_setup_script"
if file.exist(&user_setup_script)
(
    print "-- User setup found. Loading... "
    do &user_setup_script &do_not_load_menu_again
)
else
(
    print "-- No user setup found. Skipping... "
)


print "Leaving'"+os.ppf()+"'"
