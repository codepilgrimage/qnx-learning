/*
 * death_pulse.c
 *
 * QNX SDP 8.0 - Lab 05: Process Death Pulse Notification
 *
 * Demonstrates:
 *   - ChannelCreate() / ConnectAttach()
 *   - SIGEV_PULSE_INIT() / SIGEV_MAKE_UPDATEABLE()
 *   - MsgRegisterEvent()
 *   - procmgr_event_notify(PROCMGR_EVENT_PROCESS_DEATH)
 *   - MsgReceivePulse()
 *
 * Build:
 *   make                                     (uses Makefile)
 *   qcc -Vgcc_ntoaarch64le -g -Wall -o bin/death_pulse src/death_pulse.c
 *
 * Run on target:
 *   ./bin/death_pulse
 *   (then in another terminal: pidin, ls, sleep 3 & — each prints a death notice)
 *
 * Verified: QNX SDP 8.0, x86_64, QEMU VM.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <sys/siginfo.h>

#define PULSE_CODE_DEATH    1

int main(void)
{
    int             chid;
    int             coid;
    struct sigevent ev;
    struct _pulse   pulse;
    int             ret;

    /* Step 1: Create a private channel ----------------------------------- */
    chid = ChannelCreate(_NTO_CHF_PRIVATE);
    if (chid == -1) {
        perror("ChannelCreate");
        return EXIT_FAILURE;
    }
    printf("Channel created: chid=%d\n", chid);

    /* Step 2: Attach a connection to our own channel --------------------- */
    coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
    if (coid == -1) {
        perror("ConnectAttach");
        return EXIT_FAILURE;
    }
    printf("Connection attached: coid=%d\n", coid);

    /* Step 3: Initialise the pulse event --------------------------------- */
    /*   coid   = destination connection                                    */
    /*   10     = pulse priority                                            */
    /*   PULSE_CODE_DEATH = application code to identify this pulse source */
    /*   0      = initial value (overwritten at delivery by procnto)       */
    SIGEV_PULSE_INIT(&ev, coid, 10, PULSE_CODE_DEATH, 0);

    /* Step 4: Allow procnto to stamp the dead PID into pulse.value ------- */
    SIGEV_MAKE_UPDATEABLE(&ev);

    /* Step 5: Register with the System Manager --------------------------- */
    ret = MsgRegisterEvent(&ev, SYSMGR_COID);
    if (ret == -1) {
        perror("MsgRegisterEvent");
        return EXIT_FAILURE;
    }
    printf("Event registered:         ret=%d errno=%d\n", ret, errno);

    /* Step 6: Subscribe to PROCESS_DEATH events -------------------------- */
    ret = procmgr_event_notify(PROCMGR_EVENT_PROCESS_DEATH, &ev);
    if (ret == -1) {
        perror("procmgr_event_notify");
        return EXIT_FAILURE;
    }
    printf("Subscribed to PROCESS_DEATH: ret=%d errno=%d\n", ret, errno);

    /* Step 7: Block forever, draining pulses as they arrive -------------- */
    printf("\nWaiting for process death events...\n");
    printf("(run pidin / ls / sleep 3 & in another terminal)\n\n");

    for (;;) {
        ret = MsgReceivePulse(chid, &pulse, sizeof pulse, NULL);
        if (ret == -1) {
            perror("MsgReceivePulse");
            break;
        }

        if (pulse.code == PULSE_CODE_DEATH) {
            printf("Process with pid: %d died\n",
                   pulse.value.sival_int);
        } else {
            printf("Unexpected pulse code: %d value: %d\n",
                   pulse.code, pulse.value.sival_int);
        }
    }

    return EXIT_FAILURE;
}
