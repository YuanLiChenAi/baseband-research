/*
 * Copyright (c) 2001-2004, Karlsruhe University
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * Copyright (c) 2007 Open Kernel Labs, Inc. (Copyright Holder).
 * All rights reserved.
 *
 * 1. Redistribution and use of OKL4 (Software) in source and binary
 * forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 *     (a) Redistributions of source code must retain this clause 1
 *         (including paragraphs (a), (b) and (c)), clause 2 and clause 3
 *         (Licence Terms) and the above copyright notice.
 *
 *     (b) Redistributions in binary form must reproduce the above
 *         copyright notice and the Licence Terms in the documentation and/or
 *         other materials provided with the distribution.
 *
 *     (c) Redistributions in any form must be accompanied by information on
 *         how to obtain complete source code for:
 *        (i) the Software; and
 *        (ii) all accompanying software that uses (or is intended to
 *        use) the Software whether directly or indirectly.  Such source
 *        code must:
 *        (iii) either be included in the distribution or be available
 *        for no more than the cost of distribution plus a nominal fee;
 *        and
 *        (iv) be licensed by each relevant holder of copyright under
 *        either the Licence Terms (with an appropriate copyright notice)
 *        or the terms of a licence which is approved by the Open Source
 *        Initative.  For an executable file, "complete source code"
 *        means the source code for all modules it contains and includes
 *        associated build and other files reasonably required to produce
 *        the executable.
 *
 * 2. THIS SOFTWARE IS PROVIDED ``AS IS'' AND, TO THE EXTENT PERMITTED BY
 * LAW, ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, OR NON-INFRINGEMENT, ARE DISCLAIMED.  WHERE ANY WARRANTY IS
 * IMPLIED AND IS PREVENTED BY LAW FROM BEING DISCLAIMED THEN TO THE
 * EXTENT PERMISSIBLE BY LAW: (A) THE WARRANTY IS READ DOWN IN FAVOUR OF
 * THE COPYRIGHT HOLDER (AND, IN THE CASE OF A PARTICIPANT, THAT
 * PARTICIPANT) AND (B) ANY LIMITATIONS PERMITTED BY LAW (INCLUDING AS TO
 * THE EXTENT OF THE WARRANTY AND THE REMEDIES AVAILABLE IN THE EVENT OF
 * BREACH) ARE DEEMED PART OF THIS LICENCE IN A FORM MOST FAVOURABLE TO
 * THE COPYRIGHT HOLDER (AND, IN THE CASE OF A PARTICIPANT, THAT
 * PARTICIPANT). IN THE LICENCE TERMS, "PARTICIPANT" INCLUDES EVERY
 * PERSON WHO HAS CONTRIBUTED TO THE SOFTWARE OR WHO HAS BEEN INVOLVED IN
 * THE DISTRIBUTION OR DISSEMINATION OF THE SOFTWARE.
 *
 * 3. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ANY OTHER PARTICIPANT BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Description:   x86 syscall implementations
 */
#ifndef __L4__X86__SYSCALLS_H__
#define __L4__X86__SYSCALLS_H__

#include <l4/arch/vregs.h>
#include <l4/arch/segment.h>
#include <l4/arch/specials.h>
#include <l4/message.h>

/*
 * FIXME: These are unimplemented, but let wombat compile
 */
#define L4_FlushICache          29
#define L4_FlushDCache          30
#define L4_FlushCache           31

#if defined(__pic__)
# define __L4_SAVE_REGS         "       pushl %%ebx; pushl %%ebp\n"
# define __L4_RESTORE_REGS      "       popl  %%ebp; popl  %%ebx\n"
# define __L4_CLOBBER_REGS      "cc"
#else
# define __L4_SAVE_REGS         "       pushl %%ebp             \n"
# define __L4_RESTORE_REGS      "       popl  %%ebp             \n"
# define __L4_CLOBBER_REGS      "ebx", "cc"
#endif


#define __SYSCALL_SAVE_REGS \
            "       push %%ebp\n"

#define __SYSCALL_RESTORE_REGS \
            "       pop %%ebp\n"

L4_INLINE L4_ThreadId_t
L4_ExchangeRegisters(L4_ThreadId_t dest,
                     L4_Word_t control,
                     L4_Word_t sp,
                     L4_Word_t ip,
                     L4_Word_t flags,
                     L4_Word_t UserDefHandle,
                     L4_ThreadId_t pager,
                     L4_Word_t *old_control,
                     L4_Word_t *old_sp,
                     L4_Word_t *old_ip,
                     L4_Word_t *old_flags,
                     L4_Word_t *old_UserDefhandle, L4_ThreadId_t *old_pager)
{
    L4_ThreadId_t result;
    L4_Word_t dummy;
    L4_Word_t *utcb = __L4_X86_Utcb() + (__L4_TCR_SYSCALL_ARGS);

    utcb[0] = flags;
    utcb[1] = UserDefHandle;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x8000000a, %%eax\n"
            "       sysenter\n"
            "0:\n"
            "       movl %%ebp, %%ecx\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(*old_control), "=D"(*old_sp), "=b"(*old_ip),
            "=c"(*old_flags), "=d"(dummy)
            :
            "S"(dest), "d"(control), "c"(sp), "b"(ip)
            : "memory"
    );

    old_pager->raw = utcb[0];
    *old_UserDefhandle = utcb[1];

    return result;
}

L4_INLINE L4_Word_t
L4_ThreadControl(L4_ThreadId_t dest,
                 L4_SpaceId_t SpaceSpecifier,
                 L4_ThreadId_t Scheduler,
                 L4_ThreadId_t Pager,
                 L4_ThreadId_t ExceptionHandler,
                 L4_Word_t resources, void *UtcbLocation)
{
    L4_Word_t result, dummy;
    L4_Word_t *utcb = __L4_X86_Utcb() + (__L4_TCR_SYSCALL_ARGS);

    utcb[0] = ExceptionHandler.raw;
    utcb[1] = resources;
    utcb[2] = (L4_Word_t)UtcbLocation;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000006, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=b"(dummy), "=S"(dummy), "=d"(dummy), "=c"(dummy)
            :
            "S"(dest), "d"(SpaceSpecifier), "c"(Scheduler), "b"(Pager)
            : "edi"
    );

    return result;
}

L4_INLINE void
L4_ThreadSwitch(L4_ThreadId_t dest)
{
    L4_Word_t dummy;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000004, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=S"(dummy)
            :
            "S"(dest)
            :
            "eax", "ebx", "ecx", "edx", "edi"
    );
}

L4_INLINE L4_Word_t
L4_Schedule(L4_ThreadId_t dest,
            L4_Word_t timeslice,
            L4_Word_t hw_thread_bitmask,
            L4_Word_t ProcessorControl,
            L4_Word_t prio,
            L4_Word_t flags,
            L4_Word_t *rem_timeslice)
{
    L4_Word_t result, res_ts, dummy;
    L4_Word_t *utcb = __L4_X86_Utcb() + (__L4_TCR_SYSCALL_ARGS);

    utcb[0] = prio;
    utcb[1] = flags;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000009, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(res_ts), "=d"(dummy), "=c"(dummy), "=b"(dummy)
            :
            "S"(dest), "d"(timeslice), "c"(hw_thread_bitmask), "b"(ProcessorControl)
            : "edi"
    );

    if(rem_timeslice) {
        *rem_timeslice = res_ts;
    }

    return result;
}

#define L4_Lipc(to, froms, tag, from)   L4_Ipc(to, froms, tag, from)

L4_INLINE L4_MsgTag_t
L4_Ipc(L4_ThreadId_t to,
       L4_ThreadId_t FromSpecifier, L4_MsgTag_t tag, L4_ThreadId_t *from)
{
    L4_Word_t dummy;
    L4_ThreadId_t result;
    L4_MsgTag_t tag_out;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x8000000b, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(tag_out), "=d"(dummy), "=c"(dummy)
            :
            "S"(to), "d"(FromSpecifier), "c"(tag)
            :
            "edi", "ebx"
    );

    if (!L4_IsNilThread(FromSpecifier)) {
        *from = result;
    }

    return tag_out;
}

L4_INLINE L4_MsgTag_t
L4_Notify(L4_ThreadId_t to, L4_Word_t mask)
{
    L4_Word_t dummy;
    L4_MsgTag_t tag_out;
    /* XXX: optimize */
    L4_Word_t *utcb = __L4_X86_Utcb();

    utcb[0] = L4_Notifytag.raw;
    utcb[1] = mask;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x8000000b, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=S"(tag_out), "=d"(dummy), "=c"(dummy)
            :
            "S"(to), "d"(L4_nilthread), "c"(L4_Notifytag)
            :
            "eax", "ebx", "edi"
    );

    return tag_out;
}

L4_INLINE L4_MsgTag_t
L4_WaitNotify(L4_Word_t *mask)
{
    L4_Word_t dummy;
    L4_MsgTag_t tag_out;
    L4_Word_t *utcb = __L4_X86_Utcb();

    utcb[0] = L4_Waittag.raw;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x8000000b, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=S"(tag_out), "=d"(dummy), "=c"(dummy)
            :
            "S"(L4_nilthread), "d"(L4_waitnotify), "c"(L4_Waittag)
            :
            "eax", "ebx", "edi"
    );

    *mask = utcb[1];

    return tag_out;
}

L4_INLINE L4_Word_t
L4_MapControl(L4_SpaceId_t SpaceSpecifier, L4_Word_t control)
{
    L4_Word_t result, dummy;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000002, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(dummy), "=d"(dummy)
            :
            "S"(SpaceSpecifier), "d"(control)
            :
            "ebx", "ecx", "edi"
    );

    return result;
}

L4_INLINE L4_Word_t
L4_SpaceControl(L4_SpaceId_t SpaceSpecifier,
                L4_Word_t control,
                L4_ClistId_t clist,
                L4_Fpage_t UtcbArea,
                L4_Word_t resources,
                L4_Word_t *old_resources)
{
    L4_Word_t result, res_resources, dummy;
    L4_Word_t *utcb = __L4_X86_Utcb() + (__L4_TCR_SYSCALL_ARGS);

    utcb[0] = resources;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000005, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(res_resources), "=d"(dummy), "=c"(dummy), "=b"(dummy)
            :
            "S"(SpaceSpecifier), "d"(control), "c" (clist), "b"(UtcbArea)
            :
            "edi"
    );

    if(old_resources) {
        *old_resources = res_resources;
    }

    return result;
}

L4_INLINE L4_Word_t
L4_CacheControl(L4_SpaceId_t SpaceSpecifier, L4_Word_t control)
{
    L4_Word_t result, dummy;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000001, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(dummy), "=d"(dummy)
            :
            "S"(SpaceSpecifier), "d"(control)
            :
            "ebx", "ecx", "edi"
    );

    return result;
}

L4_INLINE L4_Word_t
L4_SecurityControl(L4_SpaceId_t SpaceSpecifier, L4_Word_t control)
{
    L4_Word_t result, dummy;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000003, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(dummy), "=d"(dummy)
            :
            "S"(SpaceSpecifier), "d"(control)
            :
            "ebx", "ecx", "edi"
    );

    return result;
}

L4_INLINE L4_Word_t
L4_PlatformControl(L4_Word_t control, L4_Word_t param1,
                   L4_Word_t param2, L4_Word_t param3)
{
    L4_Word_t result, dummy;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000007, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(dummy), "=d"(dummy), "=c"(dummy), "=b"(dummy)
            :
            "S"(control), "d"(param1), "c"(param2), "b"(param3)
            :
            "edi"
    );

    return result;
}

L4_INLINE L4_Word_t
L4_SpaceSwitch(L4_ThreadId_t dest, L4_SpaceId_t SpaceSpecifier,
        void *UtcbLocation)
{
    L4_Word_t result, dummy;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000008, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(dummy), "=d"(dummy), "=c"(dummy)
            :
            "S"(dest), "d"(SpaceSpecifier), "c"(UtcbLocation)
            :
            "ebx", "edi"
    );

    return result;
}

L4_INLINE word_t
L4_Mutex(L4_MutexId_t MutexSpecifier, L4_Word_t flags, word_t * state_p)
{
    L4_Word_t result, dummy;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x8000000e, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(dummy), "=d"(dummy), "=c"(dummy)
            :
            "S"(MutexSpecifier), "d"(flags), "c"(state_p)
            :
            "ebx", "edi"
    );

    return result;
}

L4_INLINE L4_Word_t
L4_MutexControl(L4_MutexId_t MutexSpecifier, L4_Word_t control)
{
    L4_Word_t result, dummy;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x8000000f, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(dummy), "=d"(dummy)
            :
            "S"(MutexSpecifier), "d"(control)
            :
            "ebx", "ecx", "edi"
    );

    return result;
}

L4_INLINE L4_Word_t
L4_InterruptControl(L4_ThreadId_t handler, L4_Word_t control)
{
    L4_Word_t result, dummy;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000010, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(dummy), "=d"(dummy)
            :
            "S"(handler), "d"(control)
            :
            "ebx", "ecx", "edi", "cc"
    );

    return result;
}

/*
 * L4_CapControl
 */
L4_INLINE L4_Word_t
L4_CapControl(L4_ClistId_t clist, L4_Word_t control)
{
    L4_Word_t result, dummy;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000011, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(dummy), "=d"(dummy)
            :
            "S"(clist), "d"(control)
            :
            "ebx", "ecx", "edi", "cc"
    );

    return result;
}

/*
 * L4_MemoryCopy
 */
L4_INLINE L4_Word_t
L4_MemoryCopy(L4_ThreadId_t remote, L4_Word_t local, L4_Word_t *size,
              L4_Word_t direction)
{
    L4_Word_t result;

    __asm__ __volatile__ (
            __SYSCALL_SAVE_REGS
            "       movl $0f, %%edi\n"
            "       movl %%esp, %%ebp\n"
            "       movl $0x80000012, %%eax\n"
            "       sysenter\n"
            "0:\n"
            __SYSCALL_RESTORE_REGS
            :
            "=a"(result), "=S"(*size)
            :
            "S"(remote), "d"(local), "c"(*size), "b"(direction)
            :
            "edi"
    );

    return result;
}


#endif /* !__L4__X86__SYSCALLS_H__ */
