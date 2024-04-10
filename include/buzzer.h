#ifndef __BUZZER__H__
#define __BUZZER__H__

void buzzer_init(void);
void buzzer_startup(void);
void buzzer_melody(int melody[], int noteDurations[], int length);
void buzzer_button_press(void);
void buzzer_success(void);
void buzzer_fail(void);
void buzzer_custom_non_blocking(int frequency, int duration);
void buzzer_custom_blocking(int frequency, int duration);

#endif