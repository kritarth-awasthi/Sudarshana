/*
 * panic_logic.h — Forward declarations for panic/alert logic
 * Implementations are in sudarshana_prototype.ino
 */

#pragma once

void trigger_panic(unsigned long now);
bool check_panic_button(unsigned long now);
void blink_led(int times, int delayMs);
void update_status_led(unsigned long now);
