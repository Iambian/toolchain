#include "fonts/fonts.h"

#include <graphx.h>
#include <fontlibc.h>
#include <keypadc.h>
#include <textioc.h>

void switch_keymaps(uint24_t *ids);
void display_keymap_indicator(uint24_t *ids);

void main(void) {

	/* Pointer to the Input Data Structure (IDS) that will
	   hold the input data */
	uint24_t *ids;

	sk_key_t key = 0;
	char *name;

	/* Setup the graphics */
	gfx_Begin();

	/* Setup our custom FontLibC font */
	fontlib_SetFont(test_font, 0);
	fontlib_SetLineSpacing(0, 0);
	fontlib_SetColors(0x00, 0xFF);

	/* Create a new IDS that will hold 8 characters and will
	   use TextIOC's program_name letter keymaps as well its numerical keymap. */
	textio_CreatePrgmNameIDS(ids, 8, 50, 50, 80);

	/* Return if a memory error occured. */
	if (ids == NULL)
		goto ERROR;

	/* Set the codepoint that will stand for the theta character. */
	textio_SetThetaCodepoint(255);

	/* Draw a box around the input field so it can be more easily seen. */
	gfx_SetColor(0x00);
	gfx_Rectangle_NoClip(48, 48, 104, 17);

	/* Set the cursor color */
	textio_SetCursorColor(ids, 0x00);

	/* Setting the cursor's height to the height of the current font ensures
	   that the cursor will be tall enough. */
	textio_SetCursorDimensions(ids, 1, fontlib_GetCurrentFontHeight() - 1);

	/* Get input until the user presses [enter] */
	do {
		display_keymap_indicator(ids);

		key = textio_Input(ids);

		/* Switch keymaps if the user presses [alpha] */
		if (key == sk_Alpha)
			switch_keymaps(ids);

	} while (key != sk_Enter);

	/* Convert the program/appvar name into a TI-OS format. */
	name = textio_ConvertProgramAppvarName_TIOS(textio_GetDataBufferPtr(ids));

	/* Draw the converted name */
	fontlib_SetCursorPosition(1, 1);
	fontlib_DrawString("Converted name: ");
	fontlib_DrawString(name);

	/* Delete the IDS. It is very important to call this function
	   when you are through with an IDS */
	textio_DeleteIDS(ids);

	/* Wait for keypress */
	while (!os_GetCSC());

	ERROR:
	gfx_End();
	exit(0);
}

void switch_keymaps(uint24_t *ids) {

	uint8_t curr_keymap_num;

	/* Get the current keymap number */
	curr_keymap_num = textio_GetCurrKeymapNum(ids);

	/* Go to the next keymap */
	if (curr_keymap_num < textio_GetNumKeymaps(ids)) {
		curr_keymap_num++;
	} else {
		/* The number of the first IDS keymap is 0 */
		curr_keymap_num = 0;
	};

	textio_SetCurrKeymapNum(ids, curr_keymap_num);

	return;
}

void display_keymap_indicator(uint24_t *ids) {

	uint8_t cursor_y;
	char indicator;

	/* Get the current IDS y-position. This y-value is the same as the cursor's y-value. */
	cursor_y = textio_GetIDSY(ids);

	/* Get the character that acts as the current keymap's indicator. */
	indicator = textio_GetCurrKeymapIndicator(ids);

	/* Draw the indicator. */
	gfx_SetColor(0x00);
	gfx_FillRectangle_NoClip(151 - fontlib_GetGlyphWidth(indicator) - 4, cursor_y - 1, fontlib_GetGlyphWidth(indicator) + 4, fontlib_GetCurrentFontHeight() + 1);

	fontlib_SetColors(0xFF, 0x00);
	fontlib_SetCursorPosition(154 - fontlib_GetGlyphWidth(indicator) - 4, cursor_y);
	fontlib_DrawGlyph(indicator);

	/* Reset the font colors */
	fontlib_SetColors(0x00, 0xFF);

	return;
}