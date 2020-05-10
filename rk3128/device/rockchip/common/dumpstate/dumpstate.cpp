/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dumpstate.h>

void dumpstate_board()
{
#ifdef TARGET_BOARD_PLATFORM_RK3399
	dump_file("Battery Statistics", "/sys/class/power_supply/test_battery/uevent");
	dump_file("touchscreen name", "/sys/class/input/input0/name");
	dump_file("dma buf info", "/d/dma_buf/bufinfo");
	dump_file("ION mali", "/d/mali/gpu_memory");
	run_command("ION CLIENTS", 5, SU_PATH, "root", "/system/bin/sh", "-c", "for f in $(ls /d/ion/clients/*); do echo $f; cat $f; done", NULL);
	run_command("ION HEAPS",   5, SU_PATH, "root", "/system/bin/sh", "-c", "for f in $(ls /d/ion/heaps/*);   do echo $f; cat $f; done", NULL);

#else
    dump_file("ion CMA heaps", "/d/ion/heaps/cma");
    dump_file("ion CMA bitmaps", "/d/ion/heaps/cma-bitmap");
    dump_file("ion CMA vmalloc", "/d/ion/heaps/vmalloc");
    dump_file("ion CMA ion_snapshot", "/d/ion/ion_snapshot");
#endif
};
