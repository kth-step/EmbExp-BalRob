/*
 * ui.h
 *
 *  Created on: 12 apr. 2019
 *      Author: Andreas Lindner
 */

#ifndef DEV_UI_H_
#define DEV_UI_H_

void ui_init();

char ui_get_button();
void ui_set_led(int i, char on);

#endif /* DEV_UI_H_ */
