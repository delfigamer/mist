	.file	"clearshape.cpp"
	.text
	.align 2
	.p2align 4,,15
	.globl	_ZN8graphics10ClearShapeD2Ev
	.def	_ZN8graphics10ClearShapeD2Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN8graphics10ClearShapeD2Ev
_ZN8graphics10ClearShapeD2Ev:
.LFB3454:
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	leaq	16+_ZTVN8graphics10ClearShapeE(%rip), %rax
	movq	%rax, (%rcx)
	movq	%rcx, %rbx
	leaq	32(%rcx), %rcx
	call	_ZN5utils8FlagLockD1Ev
	movq	%rbx, %rcx
	addq	$32, %rsp
	popq	%rbx
	jmp	_ZN8graphics5ShapeD2Ev
	.seh_endproc
	.globl	_ZN8graphics10ClearShapeD1Ev
	.def	_ZN8graphics10ClearShapeD1Ev;	.scl	2;	.type	32;	.endef
	.set	_ZN8graphics10ClearShapeD1Ev,_ZN8graphics10ClearShapeD2Ev
	.align 2
	.p2align 4,,15
	.globl	_ZN8graphics10ClearShapeD0Ev
	.def	_ZN8graphics10ClearShapeD0Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN8graphics10ClearShapeD0Ev
_ZN8graphics10ClearShapeD0Ev:
.LFB3456:
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	leaq	16+_ZTVN8graphics10ClearShapeE(%rip), %rax
	movq	%rax, (%rcx)
	movq	%rcx, %rbx
	leaq	32(%rcx), %rcx
	call	_ZN5utils8FlagLockD1Ev
	movq	%rbx, %rcx
	call	_ZN8graphics5ShapeD2Ev
	movq	%rbx, %rcx
	addq	$32, %rsp
	popq	%rbx
	jmp	_ZdlPv
	.seh_endproc
	.section .rdata,"dr"
.LC5:
	.ascii "clearshape.cpp\0"
	.text
	.align 2
	.p2align 4,,15
	.globl	_ZN8graphics10ClearShape5paintEP16IDirect3DDevice9
	.def	_ZN8graphics10ClearShape5paintEP16IDirect3DDevice9;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN8graphics10ClearShape5paintEP16IDirect3DDevice9
_ZN8graphics10ClearShape5paintEP16IDirect3DDevice9:
.LFB3457:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$112, %rsp
	.seh_stackalloc	112
	movaps	%xmm6, 96(%rsp)
	.seh_savexmm	%xmm6, 96
	.seh_endprologue
	leaq	32(%rcx), %rdi
	movq	%rcx, %rbx
	movq	%rdx, %rsi
	movq	%rdi, %rcx
	call	_ZN5utils8FlagLock4lockEv
	movss	36(%rbx), %xmm0
	movq	%rdi, %rcx
	movss	%xmm0, 64(%rsp)
	movss	40(%rbx), %xmm0
	movss	%xmm0, 68(%rsp)
	movss	44(%rbx), %xmm0
	movss	%xmm0, 72(%rsp)
	movss	48(%rbx), %xmm0
	movss	%xmm0, 76(%rsp)
	call	_ZN5utils8FlagLock6unlockEv
	movaps	64(%rsp), %xmm3
	xorl	%r8d, %r8d
	movapd	.LC1(%rip), %xmm2
	movq	%rsi, %rcx
	movl	$1, %r9d
	movaps	%xmm3, %xmm6
	cvtps2pd	%xmm3, %xmm1
	movapd	.LC2(%rip), %xmm5
	subps	.LC0(%rip), %xmm6
	andpd	%xmm2, %xmm1
	movapd	.LC3(%rip), %xmm4
	movq	(%rsi), %rax
	cvtps2pd	%xmm6, %xmm0
	andpd	%xmm2, %xmm0
	subpd	%xmm0, %xmm1
	xorps	%xmm0, %xmm0
	movhlps	%xmm3, %xmm0
	xorps	%xmm3, %xmm3
	movhlps	%xmm6, %xmm3
	cvtps2pd	%xmm0, %xmm0
	andpd	%xmm2, %xmm0
	cvtps2pd	%xmm3, %xmm3
	andpd	%xmm2, %xmm3
	subpd	%xmm3, %xmm0
	mulpd	%xmm5, %xmm1
	movq	344(%rax), %r10
	movl	$0, 48(%rsp)
	movl	$0x00000000, 40(%rsp)
	mulpd	%xmm5, %xmm0
	addpd	%xmm4, %xmm1
	addpd	%xmm4, %xmm0
	cvtpd2ps	%xmm1, %xmm1
	cvtpd2ps	%xmm0, %xmm0
	movlhps	%xmm0, %xmm1
	movaps	%xmm1, 80(%rsp)
	movss	92(%rsp), %xmm0
	movss	80(%rsp), %xmm1
	cvttss2siq	%xmm0, %rax
	movss	88(%rsp), %xmm2
	cvttss2siq	%xmm1, %rdx
	movss	84(%rsp), %xmm3
	sall	$24, %eax
	sall	$16, %edx
	orl	%edx, %eax
	cvttss2siq	%xmm2, %rdx
	orl	%edx, %eax
	cvttss2siq	%xmm3, %rdx
	sall	$8, %edx
	orl	%edx, %eax
	xorl	%edx, %edx
	movl	%eax, 32(%rsp)
	call	*%r10
	nop
	movaps	96(%rsp), %xmm6
	movl	$22, %r8d
	leaq	_ZZN8graphics10ClearShape5paintEP16IDirect3DDevice9E12__FUNCTION__(%rip), %rdx
	movl	%eax, %r9d
	leaq	.LC5(%rip), %rcx
	addq	$112, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	jmp	_ZN8graphics14checkerror_posEPKcS1_il
	.seh_endproc
	.align 2
	.p2align 4,,15
	.globl	_ZN8graphics10ClearShapeC2Ev
	.def	_ZN8graphics10ClearShapeC2Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN8graphics10ClearShapeC2Ev
_ZN8graphics10ClearShapeC2Ev:
.LFB3451:
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	movq	%rcx, %rbx
.LEHB0:
	call	_ZN8graphics5ShapeC2Ev
.LEHE0:
	leaq	16+_ZTVN8graphics10ClearShapeE(%rip), %rax
	movq	%rax, (%rbx)
	leaq	32(%rbx), %rcx
.LEHB1:
	call	_ZN5utils8FlagLockC1Ev
.LEHE1:
	movq	$0, 36(%rbx)
	movq	$0, 44(%rbx)
	addq	$40, %rsp
	popq	%rbx
	popq	%rsi
	ret
.L6:
	movq	%rax, %rsi
	movq	%rbx, %rcx
	call	_ZN8graphics5ShapeD2Ev
	movq	%rsi, %rcx
.LEHB2:
	call	_Unwind_Resume
	nop
.LEHE2:
	.def	__gxx_personality_seh0;	.scl	2;	.type	32;	.endef
	.seh_handler	__gxx_personality_seh0, @unwind, @except
	.seh_handlerdata
.LLSDA3451:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE3451-.LLSDACSB3451
.LLSDACSB3451:
	.uleb128 .LEHB0-.LFB3451
	.uleb128 .LEHE0-.LEHB0
	.uleb128 0
	.uleb128 0
	.uleb128 .LEHB1-.LFB3451
	.uleb128 .LEHE1-.LEHB1
	.uleb128 .L6-.LFB3451
	.uleb128 0
	.uleb128 .LEHB2-.LFB3451
	.uleb128 .LEHE2-.LEHB2
	.uleb128 0
	.uleb128 0
.LLSDACSE3451:
	.text
	.seh_endproc
	.globl	_ZN8graphics10ClearShapeC1Ev
	.def	_ZN8graphics10ClearShapeC1Ev;	.scl	2;	.type	32;	.endef
	.set	_ZN8graphics10ClearShapeC1Ev,_ZN8graphics10ClearShapeC2Ev
	.align 2
	.p2align 4,,15
	.globl	_ZN8graphics10ClearShape8getcolorEPf
	.def	_ZN8graphics10ClearShape8getcolorEPf;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN8graphics10ClearShape8getcolorEPf
_ZN8graphics10ClearShape8getcolorEPf:
.LFB3458:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	leaq	32(%rcx), %rdi
	movq	%rcx, %rbx
	movq	%rdx, %rsi
	movq	%rdi, %rcx
	call	_ZN5utils8FlagLock4lockEv
	movss	36(%rbx), %xmm0
	movq	%rdi, %rcx
	movss	%xmm0, (%rsi)
	movss	40(%rbx), %xmm0
	movss	%xmm0, 4(%rsi)
	movss	44(%rbx), %xmm0
	movss	%xmm0, 8(%rsi)
	movss	48(%rbx), %xmm0
	movss	%xmm0, 12(%rsi)
	addq	$32, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	jmp	_ZN5utils8FlagLock6unlockEv
	.seh_endproc
	.align 2
	.p2align 4,,15
	.globl	_ZN8graphics10ClearShape8setcolorEPKf
	.def	_ZN8graphics10ClearShape8setcolorEPKf;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN8graphics10ClearShape8setcolorEPKf
_ZN8graphics10ClearShape8setcolorEPKf:
.LFB3459:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	leaq	32(%rcx), %rdi
	movq	%rcx, %rbx
	movq	%rdx, %rsi
	movq	%rdi, %rcx
	call	_ZN5utils8FlagLock4lockEv
	movss	(%rsi), %xmm0
	movq	%rdi, %rcx
	movss	%xmm0, 36(%rbx)
	movss	4(%rsi), %xmm0
	movss	%xmm0, 40(%rbx)
	movss	8(%rsi), %xmm0
	movss	%xmm0, 44(%rbx)
	movss	12(%rsi), %xmm0
	movss	%xmm0, 48(%rbx)
	addq	$32, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	jmp	_ZN5utils8FlagLock6unlockEv
	.seh_endproc
	.section .rdata,"dr"
.LC6:
	.ascii "unknown exception\0"
	.text
	.p2align 4,,15
	.globl	_ZN8graphics23graphics_clearshape_newEv
	.def	_ZN8graphics23graphics_clearshape_newEv;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN8graphics23graphics_clearshape_newEv
_ZN8graphics23graphics_clearshape_newEv:
.LFB3460:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	xorl	%ecx, %ecx
	call	_ZN5utils14cbase_seterrorEPKc
	movl	$56, %ecx
.LEHB3:
	call	_Znwy
.LEHE3:
	movq	%rax, %rcx
	movq	%rax, %rbx
.LEHB4:
	call	_ZN8graphics5ShapeC2Ev
.LEHE4:
	leaq	16+_ZTVN8graphics10ClearShapeE(%rip), %rax
	movq	%rax, (%rbx)
	leaq	32(%rbx), %rcx
.LEHB5:
	call	_ZN5utils8FlagLockC1Ev
.LEHE5:
	movq	$0, 36(%rbx)
	movq	%rbx, %rax
	movq	$0, 44(%rbx)
.L21:
	addq	$32, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
.L18:
	movq	%rax, %rcx
.L15:
	subq	$1, %rdx
	jne	.L22
	call	__cxa_begin_catch
	movq	(%rax), %rdx
	movq	%rax, %rcx
	call	*16(%rdx)
	movq	%rax, %rcx
	call	_ZN5utils14cbase_seterrorEPKc
	call	__cxa_end_catch
	xorl	%eax, %eax
	jmp	.L21
.L20:
	movq	%rbx, %rcx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	_ZN8graphics5ShapeD2Ev
.L14:
	movq	%rbx, %rcx
	call	_ZdlPv
	movq	%rdi, %rcx
	movq	%rsi, %rdx
	jmp	.L15
.L19:
	movq	%rax, %rdi
	movq	%rdx, %rsi
	jmp	.L14
.L22:
	call	__cxa_begin_catch
	leaq	.LC6(%rip), %rcx
	call	_ZN5utils14cbase_seterrorEPKc
	call	__cxa_end_catch
	xorl	%eax, %eax
	jmp	.L21
	.seh_handler	__gxx_personality_seh0, @unwind, @except
	.seh_handlerdata
	.align 4
.LLSDA3460:
	.byte	0xff
	.byte	0x9b
	.uleb128 .LLSDATT3460-.LLSDATTD3460
.LLSDATTD3460:
	.byte	0x1
	.uleb128 .LLSDACSE3460-.LLSDACSB3460
.LLSDACSB3460:
	.uleb128 .LEHB3-.LFB3460
	.uleb128 .LEHE3-.LEHB3
	.uleb128 .L18-.LFB3460
	.uleb128 0x3
	.uleb128 .LEHB4-.LFB3460
	.uleb128 .LEHE4-.LEHB4
	.uleb128 .L19-.LFB3460
	.uleb128 0x5
	.uleb128 .LEHB5-.LFB3460
	.uleb128 .LEHE5-.LEHB5
	.uleb128 .L20-.LFB3460
	.uleb128 0x5
.LLSDACSE3460:
	.byte	0x2
	.byte	0
	.byte	0x1
	.byte	0x7d
	.byte	0
	.byte	0x7d
	.align 4
	.long	0

	.long	.LDFCM0-.
.LLSDATT3460:
	.text
	.seh_endproc
	.p2align 4,,15
	.globl	_ZN8graphics28graphics_clearshape_getcolorEPNS_10ClearShapeEPf
	.def	_ZN8graphics28graphics_clearshape_getcolorEPNS_10ClearShapeEPf;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN8graphics28graphics_clearshape_getcolorEPNS_10ClearShapeEPf
_ZN8graphics28graphics_clearshape_getcolorEPNS_10ClearShapeEPf:
.LFB3461:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, %rbx
	xorl	%ecx, %ecx
	movq	%rdx, %rsi
	call	_ZN5utils14cbase_seterrorEPKc
	leaq	32(%rbx), %rdi
	movq	%rdi, %rcx
	call	_ZN5utils8FlagLock4lockEv
	movss	36(%rbx), %xmm0
	movq	%rdi, %rcx
	movss	%xmm0, (%rsi)
	movss	40(%rbx), %xmm0
	movss	%xmm0, 4(%rsi)
	movss	44(%rbx), %xmm0
	movss	%xmm0, 8(%rsi)
	movss	48(%rbx), %xmm0
	movss	%xmm0, 12(%rsi)
	call	_ZN5utils8FlagLock6unlockEv
	movl	$1, %eax
	addq	$32, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
	.seh_endproc
	.p2align 4,,15
	.globl	_ZN8graphics28graphics_clearshape_setcolorEPNS_10ClearShapeEPKf
	.def	_ZN8graphics28graphics_clearshape_setcolorEPNS_10ClearShapeEPKf;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZN8graphics28graphics_clearshape_setcolorEPNS_10ClearShapeEPKf
_ZN8graphics28graphics_clearshape_setcolorEPNS_10ClearShapeEPKf:
.LFB3462:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, %rbx
	xorl	%ecx, %ecx
	movq	%rdx, %rsi
	call	_ZN5utils14cbase_seterrorEPKc
	leaq	32(%rbx), %rdi
	movq	%rdi, %rcx
	call	_ZN5utils8FlagLock4lockEv
	movss	(%rsi), %xmm0
	movq	%rdi, %rcx
	movss	%xmm0, 36(%rbx)
	movss	4(%rsi), %xmm0
	movss	%xmm0, 40(%rbx)
	movss	8(%rsi), %xmm0
	movss	%xmm0, 44(%rbx)
	movss	12(%rsi), %xmm0
	movss	%xmm0, 48(%rbx)
	call	_ZN5utils8FlagLock6unlockEv
	movl	$1, %eax
	addq	$32, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
	.seh_endproc
	.section .rdata,"dr"
_ZZN8graphics10ClearShape5paintEP16IDirect3DDevice9E12__FUNCTION__:
	.ascii "paint\0"
	.globl	_ZTISt9exception
	.section	.rdata$_ZTISt9exception,"dr"
	.linkonce same_size
	.align 16
_ZTISt9exception:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSSt9exception
	.globl	_ZTSSt9exception
	.section	.rdata$_ZTSSt9exception,"dr"
	.linkonce same_size
_ZTSSt9exception:
	.ascii "St9exception\0"
	.globl	_ZTIN5utils9RefObjectE
	.section	.rdata$_ZTIN5utils9RefObjectE,"dr"
	.linkonce same_size
	.align 16
_ZTIN5utils9RefObjectE:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSN5utils9RefObjectE
	.globl	_ZTSN5utils9RefObjectE
	.section	.rdata$_ZTSN5utils9RefObjectE,"dr"
	.linkonce same_size
	.align 16
_ZTSN5utils9RefObjectE:
	.ascii "N5utils9RefObjectE\0"
	.globl	_ZTIN8graphics8ResourceE
	.section	.rdata$_ZTIN8graphics8ResourceE,"dr"
	.linkonce same_size
	.align 16
_ZTIN8graphics8ResourceE:
	.quad	_ZTVN10__cxxabiv120__si_class_type_infoE+16
	.quad	_ZTSN8graphics8ResourceE
	.quad	_ZTIN5utils9RefObjectE
	.globl	_ZTSN8graphics8ResourceE
	.section	.rdata$_ZTSN8graphics8ResourceE,"dr"
	.linkonce same_size
	.align 16
_ZTSN8graphics8ResourceE:
	.ascii "N8graphics8ResourceE\0"
	.globl	_ZTIN8graphics5ShapeE
	.section	.rdata$_ZTIN8graphics5ShapeE,"dr"
	.linkonce same_size
	.align 16
_ZTIN8graphics5ShapeE:
	.quad	_ZTVN10__cxxabiv120__si_class_type_infoE+16
	.quad	_ZTSN8graphics5ShapeE
	.quad	_ZTIN8graphics8ResourceE
	.globl	_ZTSN8graphics5ShapeE
	.section	.rdata$_ZTSN8graphics5ShapeE,"dr"
	.linkonce same_size
	.align 16
_ZTSN8graphics5ShapeE:
	.ascii "N8graphics5ShapeE\0"
	.globl	_ZTSN8graphics10ClearShapeE
	.section	.rdata$_ZTSN8graphics10ClearShapeE,"dr"
	.linkonce same_size
	.align 16
_ZTSN8graphics10ClearShapeE:
	.ascii "N8graphics10ClearShapeE\0"
	.globl	_ZTIN8graphics10ClearShapeE
	.section	.rdata$_ZTIN8graphics10ClearShapeE,"dr"
	.linkonce same_size
	.align 16
_ZTIN8graphics10ClearShapeE:
	.quad	_ZTVN10__cxxabiv120__si_class_type_infoE+16
	.quad	_ZTSN8graphics10ClearShapeE
	.quad	_ZTIN8graphics5ShapeE
	.globl	_ZTVN8graphics10ClearShapeE
	.section	.rdata$_ZTVN8graphics10ClearShapeE,"dr"
	.linkonce same_size
	.align 32
_ZTVN8graphics10ClearShapeE:
	.quad	0
	.quad	_ZTIN8graphics10ClearShapeE
	.quad	_ZN8graphics10ClearShapeD1Ev
	.quad	_ZN8graphics10ClearShapeD0Ev
	.quad	_ZN8graphics8Resource7destroyEv
	.quad	_ZN8graphics5Shape9doadvanceEP16IDirect3DDevice9i
	.quad	_ZN8graphics10ClearShape5paintEP16IDirect3DDevice9
	.section .rdata,"dr"
	.align 16
.LC0:
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.align 16
.LC1:
	.long	-1
	.long	2147483647
	.long	-1
	.long	2147483647
	.align 16
.LC2:
	.long	0
	.long	1080025088
	.long	0
	.long	1080025088
	.align 16
.LC3:
	.long	0
	.long	1080033280
	.long	0
	.long	1080033280
	.data
	.align 8
.LDFCM0:
	.quad	_ZTISt9exception
	.ident	"GCC: (rev5, Built by MinGW-W64 project) 4.8.1"
	.def	_ZN5utils8FlagLockD1Ev;	.scl	2;	.type	32;	.endef
	.def	_ZN8graphics5ShapeD2Ev;	.scl	2;	.type	32;	.endef
	.def	_ZdlPv;	.scl	2;	.type	32;	.endef
	.def	_ZN5utils8FlagLock4lockEv;	.scl	2;	.type	32;	.endef
	.def	_ZN5utils8FlagLock6unlockEv;	.scl	2;	.type	32;	.endef
	.def	_ZN8graphics14checkerror_posEPKcS1_il;	.scl	2;	.type	32;	.endef
	.def	_ZN8graphics5ShapeC2Ev;	.scl	2;	.type	32;	.endef
	.def	_ZN5utils8FlagLockC1Ev;	.scl	2;	.type	32;	.endef
	.def	_Unwind_Resume;	.scl	2;	.type	32;	.endef
	.def	_ZN5utils14cbase_seterrorEPKc;	.scl	2;	.type	32;	.endef
	.def	_Znwy;	.scl	2;	.type	32;	.endef
	.def	__cxa_begin_catch;	.scl	2;	.type	32;	.endef
	.def	__cxa_end_catch;	.scl	2;	.type	32;	.endef
	.def	_ZN8graphics8Resource7destroyEv;	.scl	2;	.type	32;	.endef
	.def	_ZN8graphics5Shape9doadvanceEP16IDirect3DDevice9i;	.scl	2;	.type	32;	.endef
