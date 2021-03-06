/*
 * Copyright (c) 2014, Intel Corporation
 * All rights reserved.
 *
 * Author: Andrew Boie <andrew.p.boie@intel.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <efi.h>
#include <efiapi.h>
#include <efilib.h>

#include "ux.h"
#include "ui.h"
#include "lib.h"
#include "unittest.h"

static VOID test_keys(VOID)
{
        const UINTN wait_s = 10;
        UINTN i;
        ui_events_t event;

        Print(L"Reading keys for the next %d seconds...\n", wait_s);
        for (i = 0; i <= wait_s * 1000; i += 1) {
                event = ui_read_input();
                if (event == EV_NONE) {
                        uefi_call_wrapper(BS->Stall, 1, 1000);
                        continue;
                }
                Print(L"Received %d key event\n", event);
        }
}

static UINT8 fake_hash[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB};

static VOID test_ux(VOID)
{
        /* TODO: some method of programmatically verifying that these work */
        ux_prompt_user_bootimage_unverified();
        ux_warn_user_unverified_recovery();
        ux_prompt_user_device_unlocked();
        ux_prompt_user_secure_boot_off();
        ux_prompt_user_keystore_unverified(fake_hash);
        ux_crash_event_prompt_user_for_boot_target();
}

static struct test_suite {
        CHAR16 *name;
        VOID (*fun)(VOID);
} TEST_SUITES[] = {
        { L"ux", test_ux },
        { L"keys", test_keys }
};

VOID unittest_main(CHAR16 *testname)
{
        BOOLEAN found = FALSE;
        UINTN i;

        for (i = 0; i < ARRAY_SIZE(TEST_SUITES); i++)
                if (!testname || !StrCmp(L"all", testname) ||
                    !StrCmp(TEST_SUITES[i].name, testname)) {
                        found = TRUE;
                        Print(L"'%s' test suite begins\n", TEST_SUITES[i].name);
                        TEST_SUITES[i].fun();
                        Print(L"'%s' test suite terminated\n", TEST_SUITES[i].name);
                }

        if (!found)
                Print(L"'%s' test suite not found\n", testname);
}
