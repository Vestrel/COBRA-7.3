	.file	"main.c"
	.section	".toc","aw"
	.section	".text"
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	".text"
.Ltext0:
	.section	".toc","aw"
.LC1:
	.tc .LC0[TC],.LC0
.LC2:
	.tc lv2_printf[TC],lv2_printf
.LC4:
	.tc .LC3[TC],.LC3
	.section	".text"
	.align 2
	.globl main
	.section	".opd","aw"
	.align 3
main:
	.quad	.L.main,.TOC.@tocbase,0
	.previous
	.type	main, @function
.L.main:
.LFB26:
	.file 1 "main.c"
	.loc 1 22 0
	mflr 0
.LCFI0:
	std 31,-8(1)
.LCFI1:
	.loc 1 23 0
	nop
	nop
	ld 31,.LC2@toc(2)
	ld 3,.LC1@toc(2)
	.loc 1 22 0
	std 0,16(1)
.LCFI2:
	stdu 1,-128(1)
.LCFI3:
	.loc 1 23 0
	std 2,40(1)
	nop
	ld 0,0(31)
	ld 11,16(31)
	ld 2,8(31)
	mtctr 0
	bctrl
	ld 2,40(1)
	.loc 1 25 0
	bl init_syscall_info
	nop
	cmpdi 7,3,0
	bne 7,.L10
	.loc 1 28 0
	bl sc_sys_hid_init
	nop
	cmpdi 7,3,0
	bne 7,.L10
	.loc 1 31 0
	bl sc_sys_config_init
	nop
	cmpdi 7,3,0
	bne 7,.L10
	.loc 1 34 0
	bl sc_sys_event_queue_init
	nop
	cmpdi 7,3,0
	bne 7,.L10
	.loc 1 37 0
	bl init_syscall_pool
	nop
	cmpdi 7,3,0
	bne 7,.L10
	.loc 1 40 0
	bl init_syscall_writer
	nop
	cmpdi 7,3,0
	bne 7,.L10
	.loc 1 43 0
	bl init_syscall_handler
	nop
	cmpdi 7,3,0
	bne 7,.L10
	.loc 1 46 0
	std 2,40(1)
	ld 3,.LC4@toc(2)
	nop
	nop
	ld 0,0(31)
	ld 11,16(31)
	ld 2,8(31)
	mtctr 0
	bctrl
	ld 2,40(1)
.L10:
	.loc 1 47 0
	addi 1,1,128
	ld 0,16(1)
	ld 31,-8(1)
	mtlr 0
	blr
	.long 0
	.byte 0,0,0,1,128,1,0,0
.LFE26:
	.size	main,.-.L.main
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 3
.LC0:
	.string	"SCH: Initializing...\n\n"
	.zero	1
.LC3:
	.string	"SCH: Initialization complete.\n\n"
	.section	.debug_frame,"",@progbits
.Lframe0:
	.4byte	.LECIE0-.LSCIE0
.LSCIE0:
	.4byte	0xffffffff
	.byte	0x1
	.string	""
	.uleb128 0x1
	.sleb128 -8
	.byte	0x41
	.byte	0xc
	.uleb128 0x1
	.uleb128 0x0
	.align 3
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.8byte	.LFB26
	.8byte	.LFE26-.LFB26
	.byte	0x4
	.4byte	.LCFI1-.LFB26
	.byte	0x9f
	.uleb128 0x1
	.byte	0x9
	.uleb128 0x41
	.uleb128 0x0
	.byte	0x4
	.4byte	.LCFI3-.LCFI1
	.byte	0xe
	.uleb128 0x80
	.byte	0x11
	.uleb128 0x41
	.sleb128 -2
	.align 3
.LEFDE0:
	.section	".text"
.Letext0:
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.8byte	.LFB26-.Ltext0
	.8byte	.LCFI3-.Ltext0
	.2byte	0x1
	.byte	0x51
	.8byte	.LCFI3-.Ltext0
	.8byte	.LFE26-.Ltext0
	.2byte	0x3
	.byte	0x71
	.sleb128 128
	.8byte	0x0
	.8byte	0x0
	.section	.debug_info
	.4byte	0x97
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.4byte	.LASF10
	.byte	0x1
	.4byte	.LASF11
	.4byte	.LASF12
	.8byte	.Ltext0
	.8byte	.Letext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF2
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF3
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF4
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF5
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF7
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF8
	.uleb128 0x4
	.byte	0x8
	.byte	0x7
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF9
	.uleb128 0x5
	.byte	0x1
	.4byte	.LASF13
	.byte	0x1
	.byte	0x16
	.byte	0x1
	.8byte	.LFB26
	.8byte	.LFE26
	.4byte	.LLST0
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",@progbits
	.4byte	0x17
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x9b
	.4byte	0x7d
	.string	"main"
	.4byte	0x0
	.section	.debug_aranges,"",@progbits
	.4byte	0x2c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x8
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.8byte	.Ltext0
	.8byte	.Letext0-.Ltext0
	.8byte	0x0
	.8byte	0x0
	.section	.debug_str,"MS",@progbits,1
.LASF4:
	.string	"unsigned int"
.LASF12:
	.string	"D:\\RPCS3\\COBRA-7.3\\484\\DEH\\syscall_hooks"
.LASF6:
	.string	"long unsigned int"
.LASF0:
	.string	"signed char"
.LASF3:
	.string	"short unsigned int"
.LASF2:
	.string	"short int"
.LASF9:
	.string	"long long unsigned int"
.LASF11:
	.string	"main.c"
.LASF1:
	.string	"unsigned char"
.LASF5:
	.string	"long int"
.LASF13:
	.string	"main"
.LASF10:
	.string	"GNU C 4.2.0"
.LASF7:
	.string	"char"
.LASF8:
	.string	"long long int"
	.ident	"GCC: (GNU) 4.2.0"
