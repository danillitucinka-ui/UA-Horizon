; ============================================================================
; UA-HORIZON OS BOOTLOADER
; ============================================================================
; Real Mode -> Protected Mode -> Long Mode (64-bit)
; National OS Bootloader for Ukraine
; 
; Magic number: 0xUA1991 (Ukraine 1991 - Independence Day)
; ============================================================================

[bits 16]
[org 0x7c00]

; ====================
; MULTIBOOT HEADER
; ====================
align 4
mboot_header:
    dd 0x1badb002           ; Multiboot magic number
    dd 0x00000003           ; Flags: AOUT kludge, memory info, video mode
    dd -(0x1badb002 + 0x00000003)  ; Checksum
    dd mboot_header         ; Header address
    dd 0x7c00              ; Load address
    dd 0x7c00              ; Load end address
    dd 0x9a000             ; BSS end address
    dd _start              ; Entry point

; ====================
; GDT (Global Descriptor Table) - 32-bit
; ====================
section .data
align 16
gdt:
    ; Null descriptor (required)
    dq 0x0000000000000000
    
    ; Code segment (64-bit): base=0, limit=0xFFFFF, G=1, D/B=0 (64-bit), P=1, DPL=0, S=1, Type=1010 (Code)
    dw 0xFFFF               ; Limit low
    dw 0x0000               ; Base low
    db 0x00                 ; Base middle
    db 0b10011010           ; Access byte: P=1, DPL=0, S=1, Type=Code
    db 0b10101111           ; Flags: G=1, D/B=0 (64-bit), L=1, AVL=0 (high nibble), Limit high
    db 0x00                 ; Base high
    
    ; Data segment: base=0, limit=0xFFFFF, G=1, D/B=1, P=1, DPL=0, S=1, Type=0010 (Data)
    dw 0xFFFF               ; Limit low
    dw 0x0000               ; Base low
    db 0x00                 ; Base middle
    db 0b10010010           ; Access byte: P=1, DPL=0, S=1, Type=Data
    db 0b11001111           ; Flags: G=1, D/B=1, L=0, AVL=0
    db 0x00                 ; Base high

gdt_end:

gdt_ptr:
    dw gdt_end - gdt - 1    ; GDT limit
    dq gdt                  ; GDT base address

; ====================
; IDT (Interrupt Descriptor Table) - 64-bit Long Mode
; ====================
align 16
idt:
    times 256 dq 0          ; 256 x 16-byte entries (will be filled later)

idt_ptr:
    dw (256 * 16) - 1       ; IDT limit
    dq idt                  ; IDT base address

; ====================
; PAGE TABLES for Long Mode
; ====================
align 4096
pml4:
    dq pdp + 0x3            ; PML4[0] points to PDP with P=1, R/W=1
    times 511 dq 0

pdp:
    dq pdt + 0x3            ; PDP[0] points to PDT with P=1, R/W=1
    times 511 dq 0

pdt:
    dq pt + 0x3             ; PDT[0] points to PT with P=1, R/W=1
    times 511 dq 0

; Page table: Identity map first 2MB
pt:
    %assign i 0
    %rep 512
        dq (i * 4096) + 0x3 ; P=1, R/W=1
        %assign i i+1
    %endrep

; ====================
; BOOT CODE
; ====================
section .text
_start:
    cli                     ; Disable interrupts
    cld                     ; Clear direction flag
    
    ; Initialize DS, ES, SS
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    
    ; ====================
    ; Step 1: Enable A20 Line (using fast method)
    ; ====================
    call enable_a20
    
    ; ====================
    ; Step 2: Load GDT
    ; ====================
    lgdt [gdt_ptr]
    
    ; ====================
    ; Step 3: Switch to Protected Mode (32-bit)
    ; ====================
    mov eax, cr0
    or eax, 1               ; Set PE (Protection Enable) bit
    mov cr0, eax
    
    ; Far jump to flush prefetch queue and switch to Protected Mode
    jmp 0x08:protected_mode_entry
    
    align 4
protected_mode_entry:
    [bits 32]
    ; Setup segment registers for Protected Mode
    mov ax, 0x10            ; Data segment selector
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    
    ; ====================
    ; Step 4: Setup paging for identity mapping
    ; ====================
    mov eax, pml4
    mov cr3, eax            ; Load PML4 into CR3
    
    ; ====================
    ; Step 5: Enable Physical Address Extension (PAE)
    ; ====================
    mov eax, cr4
    or eax, 0x00000020      ; Set PAE bit (bit 5)
    mov cr4, eax
    
    ; ====================
    ; Step 6: Enable Long Mode (IA-32e Mode)
    ; ====================
    mov ecx, 0xC0000080     ; IA32_EFER MSR
    rdmsr
    or eax, 0x00000100      ; Set LM (Long Mode) bit
    wrmsr
    
    ; ====================
    ; Step 7: Enable paging (CR0.PG)
    ; ====================
    mov eax, cr0
    or eax, 0x80000000      ; Set PG (Paging) bit
    mov cr0, eax
    
    ; Far jump to 64-bit code
    jmp 0x08:long_mode_entry
    
    align 4
long_mode_entry:
    [bits 64]
    ; Clear upper bits of RSP
    mov rax, qword [rel stack_top]
    mov rsp, rax
    
    ; Zero out .bss section
    mov rdi, 0x100000       ; Kernel load address
    mov rcx, 0x4000         ; 16KB for BSS
    xor rax, rax
    rep stosq
    
    ; Load IDT for 64-bit
    lidt [rel idt_ptr]
    
    ; Jump to kernel at 0x100000 (kernel load address)
    jmp 0x100000
    
    ; Infinite loop (fallback)
    hlt
    jmp $

; ====================
; PROCEDURES
; ====================

; Enable A20 line using fast method (FAST A20)
enable_a20:
    [bits 16]
    in al, 0x92             ; Read status byte
    or al, 0x02             ; Set A20 gate bit
    and al, 0xFE            ; Clear Reset bit
    out 0x92, al
    ret

; ====================
; DATA SECTION
; ====================
section .data
    stack_top: dq 0x120000  ; Stack pointer (128KB offset)

; ====================
; BOOT SIGNATURE
; ====================
section .rodata
    times (510 - ($ - $$)) db 0
    db 0x55, 0xAA           ; Boot signature

; ====================
; KERNEL CODE (will be linked with C/Rust)
; ====================
section .text
kernel_start:
    ; This location will be linked with kernel_main from C/Rust
