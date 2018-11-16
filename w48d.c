#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include  <sys/types.h>

pid_t fork(void);

#define ABGEHOBEN 1
#define AUFGELEGT 0

#define AKTIV 1
#define INAKTIV 0

#define PIN_NSA 8 // Nummern-Schalter-Arbeits- (oder Abschalte-)Kontakt
#define PIN_NSI 9 // Nummern-Schalter-Impuls-Kontakt
#define PIN_GU 7  // Gabelumschalter

volatile unsigned long alteZeitNSA=0, entprellZeitNSA=100;
volatile unsigned long alteZeitNSI=0, entprellZeitNSI=100;
volatile unsigned long alteZeitGU=0, entprellZeitGU=100;

int impulse =0;
int StatusNSA = 0;
int StatusGU = 0; // Gabelumschalter ist auf High wenn aufgelegt ist. Hoerer abgenommen ist LOW


int DialCnt = 0;
int DialStatus = 0;

char number[20] = { '\0' };

void playFree(void);
void playImpulse(void);
void StopFree(void);

void dial(void);
void hangup(void);

// -------------------------------------------------------------------------
// myInterrupt:  called every time an event occurs
void InterruptGU(void) {
    if((millis() - alteZeitGU) > entprellZeitGU) { 
        if (digitalRead (PIN_GU) == ABGEHOBEN && StatusGU == AUFGELEGT) {
            printf("Hoerer abgehoben %d\n",millis());
            StatusGU = ABGEHOBEN;
            playFree();
        } else if (digitalRead (PIN_GU) == AUFGELEGT && StatusGU == ABGEHOBEN) {
            printf("Hoerer aufgelegt %d\n",millis());
            StatusGU = AUFGELEGT;
            DialStatus = 0;
            StopFree();
            hangup();
        }
        alteZeitGU = millis(); // letzte Schaltzeit merken      
    }
}


void InterruptNSA(void) {
    if((millis() - alteZeitNSA) > entprellZeitNSA) { 
        if (StatusGU == ABGEHOBEN && StatusNSA == INAKTIV) {
            printf("Waehlscheibe start %d\n",millis());
            StatusNSA = AKTIV;
        StopFree();
            DialCnt=0;
        } else if (StatusGU == ABGEHOBEN && StatusNSA == AKTIV) {
            printf("Waehlscheibe ende %d\n",millis());
            if (impulse == 10) impulse = 0;
            sprintf(number, "%s%d", number, impulse);
            impulse = 0;
            StatusNSA = INAKTIV;
            DialCnt=0;
            printf("Number %s \n",number);
        }
        alteZeitNSA = millis(); // letzte Schaltzeit merken      
    }
}

void InterruptNSI(void) {
    if(((millis() - alteZeitNSI) > entprellZeitNSI) && (StatusGU == ABGEHOBEN) ) { 

        impulse++;
        DialCnt=0;
        printf("Impuls #%d %d\n",impulse,millis());

        playImpulse();
        alteZeitNSI = millis(); // letzte Schaltzeit merken      
    }
}

void dial (void) {

    printf("DIAL %s\n",number);
    DialStatus = 1;

        StopFree();

   pid_t pid;

    if((pid = fork()) < 0) {
        fprintf(stderr, "Fehler... %s\n", strerror(errno));
    } else if(pid == 0) {
    /* Kindprozess */
    char command[100];
    char *environment[4];
    environment[0] = "SHELL=/bin/bash";
    environment[1] = "LOGNAME=w48dialerd";
    environment[2] = "OSTYPE=linux";
    environment[3] = NULL;
    //execle("child.exe", "child", "arg1", "arg2", NULL, env);
// asterisk -rx 'console dial 1001'

    sprintf(command, "console dial %s", number);

    execle("/usr/sbin/asterisk", "asterisk", "-rx", command,NULL, NULL, environment);
    } 
    number[0] = '\0';
}

void hangup (void) {

    printf("Hangup\n");
    DialStatus = 0;

   pid_t pid;

    if((pid = fork()) < 0) {
        fprintf(stderr, "Fehler... %s\n", strerror(errno));
    } else if(pid == 0) {
    /* Kindprozess */
    char command[100];
    char *environment[4];
    environment[0] = "SHELL=/bin/bash";
    environment[1] = "LOGNAME=w48dialerd";
    environment[2] = "OSTYPE=linux";
    environment[3] = NULL;
    sprintf(command, "console hangup");
    execle("/usr/sbin/asterisk", "asterisk", "-rx", command,NULL, NULL, environment);
    } 
    number[0] = '\0';
}

void playFree (void) {

   pid_t pid;

    if((pid = fork()) < 0) {
        fprintf(stderr, "Fehler... %s\n", strerror(errno));
    } else if(pid == 0) {
    /* Kindprozess */
    char command[100] = "";
    char *environment[4];
    environment[0] = "SHELL=/bin/bash";
    environment[1] = "LOGNAME=w48play";
    environment[2] = "OSTYPE=linux";
    environment[3] = NULL;
    //execle("child.exe", "child", "arg1", "arg2", NULL, env);
// asterisk -rx 'console dial 1001'

//    speaker-test -c1 -t sine -f 425

    execle("/usr/sbin/w48play", "w48play", "1", command,NULL, NULL, environment);
    } 
//    number[0] = '\0';
}

void playImpulse (void) {

   pid_t pid;

    if((pid = fork()) < 0) {
        fprintf(stderr, "Fehler... %s\n", strerror(errno));
    } else if(pid == 0) {
    /* Kindprozess */
    char command[100] = "";
    char *environment[4];
    environment[0] = "SHELL=/bin/bash";
    environment[1] = "LOGNAME=w48play";
    environment[2] = "OSTYPE=linux";
    environment[3] = NULL;
    //execle("child.exe", "child", "arg1", "arg2", NULL, env);
// asterisk -rx 'console dial 1001'

//    speaker-test -c1 -t sine -f 425

    execle("/usr/sbin/w48play", "w48play", "3", command,NULL, NULL, environment);
    } 
//    number[0] = '\0';
}


void StopFree (void) {

   pid_t pid;

    if((pid = fork()) < 0) {
        fprintf(stderr, "Fehler... %s\n", strerror(errno));
    } else if(pid == 0) {
    /* Kindprozess */
    char *environment[4];
    environment[0] = "SHELL=/bin/bash";
    environment[1] = "LOGNAME=w48play";
    environment[2] = "OSTYPE=linux";
    environment[3] = NULL;

    execle("/usr/bin/killall", "killall", "w48play", NULL,NULL, NULL, environment);
    } 
}



int main (int argc, char *argv[], char **env_var_ptr) {

    int i=0;
    while (*env_var_ptr != NULL) {
        i++;
        printf ("\nenv var%d=>%s",i, *(env_var_ptr++));
    }
    printf("\n");


  if (wiringPiSetup () < 0) {
      fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
      return 1;
  }

    pinMode(PIN_NSA,INPUT);
    pinMode(PIN_NSI,INPUT);
    pinMode(PIN_GU,INPUT);

    pullUpDnControl (PIN_NSA, PUD_UP) ;
    pullUpDnControl (PIN_NSI, PUD_UP) ;
    pullUpDnControl (PIN_GU, PUD_UP) ;


  if ( wiringPiISR (PIN_NSA, INT_EDGE_BOTH, &InterruptNSA) < 0 ) {
      fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
      return 1;
  }

  if ( wiringPiISR (PIN_NSI, INT_EDGE_RISING, &InterruptNSI) < 0 ) {
      fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
      return 1;
  }

  if ( wiringPiISR (PIN_GU, INT_EDGE_BOTH, &InterruptGU) < 0 ) {
      fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
      return 1;
  }



  while ( 1 ) {

    if (StatusGU == ABGEHOBEN && strlen(number) >= 1 && DialCnt >= 5) {
        if (DialStatus == 0) {
        dial();
        } else {
//        sendDTMF();
        }

    }

    if (StatusGU == AUFGELEGT) {
        DialCnt =0;
        number[0] = '\0';
    }


    if (DialCnt >= 100) DialCnt = 0;

    delay( 500 ); // wait 1 second
    DialCnt++;
  }

  return 0;
}
