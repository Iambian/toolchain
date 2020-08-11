#include "fonts/fonts.h"

#include <graphx.h>
#include <fontlibc.h>
#include <keypadc.h>
#include <textioc.h>

void setup_fontlib_textio(void);
void switch_keymaps(uint24_t *ids);
void display_keymap_indicator(uint24_t *ids, uint8_t color);

void main(void) {

	/* Pointers to the Input Data Structures (IDS) that will
	   hold the input data. */
	uint24_t *ids_master, *ids_one, *ids_two;
	sk_key_t key = 0;
	uint8_t color;

	/* Setup the graphics. */
	gfx_Begin();

	/* Setup our custom FontLibC font. */
	fontlib_SetFont(test_font, 0);
	fontlib_SetColors(0x00, 0xFF);

	/* Set FontLibC as TextIOC's source library. */
	setup_fontlib_textio();

	/* Create two new IDS that will each hold 10 characters and will
	   use TextIOC's uppercase and lowercase keymaps. */
	textio_CreateAlphaIDS(ids_one, 10, 50, 50, 80);
	textio_CreateAlphaIDS(ids_two, 10, 50, 100, 80);

	/* Return if a memory error occurred. */
	if (ids_one == NULL || ids_two == NULL)
		goto ERROR;

	/* Set the cursor colors. */
	textio_SetCursorColor(ids_one, 0x00);
	textio_SetCursorColor(ids_two, 0x00);

	/* Setting the cursor's height to the height of the current font ensures
	   that the cursor will be tall enough. */
	textio_SetCursorDimensions(ids_one, 1, fontlib_GetCurrentFontHeight());
	textio_SetCursorDimensions(ids_two, 1, fontlib_GetCurrentFontHeight());

	/* Set the cursor y-positions. */
	textio_SetCursorY(ids_one, textio_GetIDSY(ids_one));
	textio_SetCursorY(ids_two, textio_GetIDSY(ids_two));

	/* Increase the cursor blink rate. */
	textio_SetCursorBlinkRate(10);

	/* Set the first active IDS. */
	ids_master = ids_one;
	textio_LockIDS(ids_two, true);

	/* Get input until the user presses [enter] */
	do {
		/* Draw boxes around the input fields and the keymap indicators
		   based on the IDS_Lock status for each IDS. */
		color = 0x00;
		if (textio_GetIDSLock(ids_one))
			color = 0xb5;
		gfx_SetColor(color);
		gfx_Rectangle_NoClip(48, 48, 104, 18);
		display_keymap_indicator(ids_one, color);

		color = 0x00;
		if (textio_GetIDSLock(ids_two))
			color = 0xb5;
		gfx_SetColor(color);
		gfx_Rectangle_NoClip(48, 98, 104, 18);
		display_keymap_indicator(ids_two, color);

		key = textio_Input(ids_master);

		/* Switch keymaps if the user presses [alpha] */
		if (key == sk_Alpha)
			switch_keymaps(ids_master);

		/* Switch input fields if requested. */
		if (key == sk_Up) {
			textio_LockIDS(ids_two, true);
			textio_LockIDS(ids_one, false);
			ids_master = ids_one;
		};

		if (key == sk_Down) {
			textio_LockIDS(ids_one, true);
			textio_LockIDS(ids_two, false);
			ids_master = ids_two;
		};

	} while (key != sk_Enter);

	/* Delete both IDSes. */
	textio_DeleteIDS(ids_one);
	textio_DeleteIDS(ids_two);

	ERROR:
	gfx_End();
	exit(0);
}

void setup_fontlib_textio(void) {

	/* Setup the FontLibC wrapper. */
	textio_library_routines_t routines = TEXTIO_FONTLIB_ROUTINES;

	/* Pass the wrapper pointers to TextIOC. */
	textio_SetLibraryRoutines(&routines);
	return;
}

void switch_keymaps(uint24_t *ids) {

	uint8_t curr_keymap_num;

	/* Get the current keymap number. */
	curr_keymap_num = textio_GetCurrKeymapNum(ids);

	/* Go to the next keymap. */
	if (curr_keymap_num < textio_GetNumKeymaps(ids)) {
		curr_keymap_num++;
	} else {
		/* The number of the first IDS keymap is 0. */
		curr_keymap_num = 0;
	};

	textio_SetCurrKeymapNum(ids, curr_keymap_num);

	return;
}

void display_keymap_indicator(uint24_t *ids, uint8_t color) {

	uint8_t cursor_y;
	char indicator;

	/* Get the cursor's y-position. */
	cursor_y = textio_GetIDSY(ids);

	/* Get the character that acts as the current keymap's indicator. */
	indicator = textio_GetCurrKeymapIndicator(ids);

	/* Draw the indicator. */
	gfx_FillRectangle_NoClip(139, cursor_y - 1, 12, fontlib_GetCurrentFontHeight() + 2);

	fontlib_SetColors(0xFF, color);
	fontlib_SetCursorPosition(154 - fontlib_GetGlyphWidth(indicator) - 4, cursor_y + 1);
	fontlib_DrawGlyph(indicator);

	/* Reset the font colors */
	fontlib_SetColors(0x00, 0xFF);

	return;
}