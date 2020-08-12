/**
 * @file
 * @brief Contains text-wrapping and text input routines for GraphX and FontLibC.
 *
 * TextIOC is a general-purpose text I/O library, supporting both GraphX and FontLibC. *Please
 * note that the OS homescreen is not supported in version 2.0 of this library.*
 *
 * The library's text input is based on the Input Data Structure (IDS) which holds the input
 * data and configuration data for the two input routines, textio_Input and textio_TimedInput.
 * Each IDS is at least 27 bytes plus the sum of the number of characters it is to hold and
 * the number of keymaps it uses times three. Each IDS can use up to 255 keymaps although it is
 * recommended for the sake of saving memory that a much lower number of keymaps be allocated at
 * any one time.
 *
 * TextIOC offers four built-in keymaps, two for uppercase and lowercase characters, one for numerals,
 * and one for program/appvar names. It also supports custom keymaps created by the programmer.
 *
 * Each keymap is 57 bytes long. The first character of the keymap acts as the "keymap indicator,"
 * a special character that represents the keymap. The keymap data format is identical to the one
 * used as an example for os_GetCSC in the *tice.h* documentation.
 *
 * The first keymap in an IDS has an index of zero. The next one has an index of one, and so on.
 * Thus, the number of keymaps in the IDS is the actual number of keymaps minus one. This is
 * important to remember when changing keymaps.
 *
 * Both input routines automatically exit after each keypress, so in order to get more than one
 * character of input, it is necessary to put them inside of a *while {...}* or *do {...} while* loop.
 * The textio_TimedInput function also automatically exits after each second which allows the
 * programmer to print or to check the timer's status.
 *
 * The general outline of an textio_Input call resembles the following:
 * @code
 * uint24_t *ids;
 * char *keymap;
 * sk_key_t key;
 *
 * ids = textio_CreateIDS(10, 50, 50, 50, 1);    // Create an IDS
 *
 * // Create a custom keymap and assign it to an IDS (optional)
 * keymap = textio_CreateKeymap('A', \0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0);
 * textio_SetKeymaps(ids, 1, keymap);
 *
 * textio_SetCursorDimensions(ids, 1, 9);    // Set the cursor dimensions
 *
 * // Do additional configuration for the cursor or IDS (optional)
 *
 * // An input loop
 * do {
 *	 key = textio_Input(ids);
 * } while (key != escape_key);
 *
 * // Do something with the input
 *
 * // Free each IDS created
 * textio_DeleteIDS(ids);
 *
 * // Free any keymaps created
 * textio_DeleteKeymap(keymap);
 * @endcode
 *
 * TextIOC uses a text window for displaying text output like FontLibC. This window and its associated
 * functions, such as textio_SetNewlineCode and textio_SetLineSpacing, are **completely** seperate from
 * the FontLib text window. Thus, if the programmer is using FontLibC, he will need to setup the TextIOC
 * text window as he would the FontLibC window. This also means that it is safe to use both a TextIOC text
 * window and and FontLib text window on the same screen simultaneously.
 *
 * TextIOC offers a SetLineSpacing function that acts exactly like its FontLib counterpart. The FontLib
 * function, fontlib_SetLineSpacing, does not affect the TextIOC text window. fontlib_SetFirstPrintableCodePoint,
 * however, does apply to the TextIOC text window if TextIOC is setup to use the FontLib library.
 *
 *
 * @authors "Captain Calc"
 *
 * Many thanks to DrDnar, jacobly, Adriweb, and the other members of the CE Programming team for their
 * help and support!
 */


#ifndef H_TEXTIOC
#define H_TEXTIOC

#include <graphx.h>
#include <fontlibc.h>
#include <stdint.h>
#include <tice.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This structure holds the pointers to the external text functions as well as the library version data.
 *
 * @see textio_SetLibraryRoutines
*/
typedef struct {
	uint8_t library_version;
	void *set_text_position;
	void *get_text_x;
	void *get_text_y;
	void *draw_char;
	void *get_char_width;
} textio_library_routines_t;

/**
 * Function wrappers for FontLibC.
*/
static void textio_fontlib_SetCursorPosition(uint24_t xPos, uint8_t yPos) {
	fontlib_SetCursorPosition((unsigned int)xPos, yPos);
	return;
}

static uint24_t textio_fontlib_GetCursorX(void) {
	return (uint24_t)fontlib_GetCursorX();
}

static uint24_t textio_fontlib_GetCursorY(void) {
	return (uint24_t)fontlib_GetCursorY();
}

static void textio_fontlib_DrawGlyph(char codepoint) {
	fontlib_DrawGlyph((uint8_t)codepoint);
	return;
}

static uint24_t textio_fontlib_GetGlyphWidth(char codepoint) {
	return (uint24_t)fontlib_GetGlyphWidth(codepoint);
}

/**
 * Function wrappers for GraphX.
 */
static void textio_gfx_SetTextXY(uint24_t xPos, uint8_t yPos) {
	gfx_SetTextXY((int)xPos, (int)yPos);
	return;
}

static uint24_t textio_gfx_GetTextX(void) {
	return (uint24_t)gfx_GetTextX();
}

static uint24_t textio_gfx_GetTextY(void) {
	return (uint24_t)gfx_GetTextY();
}

static void textio_gfx_PrintChar(char codepoint) {
	gfx_PrintChar((const char)codepoint);
	return;
}

static uint24_t textio_gfx_GetCharWidth(char codepoint) {
	return (uint24_t)gfx_GetCharWidth((const char)codepoint);
}

/**
 * Default external function pointers for FontLibC.
*/
#define TEXTIO_FONTLIB_ROUTINES { \
				2, \
				textio_fontlib_SetCursorPosition, \
				textio_fontlib_GetCursorX, \
				textio_fontlib_GetCursorY, \
				textio_fontlib_DrawGlyph, \
				textio_fontlib_GetGlyphWidth \
				};

/**
 * Default external function pointers for GraphX.
*/
#define TEXTIO_GRAPHX_ROUTINES { \
				2, \
				textio_gfx_SetTextXY, \
				textio_gfx_GetTextX, \
				textio_gfx_GetTextY, \
				textio_gfx_PrintChar, \
				textio_gfx_GetCharWidth \
				};

/**
 * Provides the external text function pointers to TextIOC.
 *
 * @note The recommended method for using this function is shown below:
 * @code
 * textio_library_routines_t routines = TEXTIO_GRAPHX_ROUTINES;
 * // Or, textio_library_routines_t routines = TEXTIO_FONTLIB_ROUTINES;
 * 
 * textio_SetLibraryRoutines(&routines);
 * @endcode
 * @see textio_library_routines_t
 * 
 * @param ptr Pointer to routine structure
*/
void textio_SetLibraryRoutines(textio_library_routines_t *ptr);

/**
 * Allocates memory for a *Input Data Structure (IDS)*.
 *
 * @note
 * Unless a different memory allocation routine other than \c malloc()
 * needs to be used, textio_CreateIDS should be used.
 *
 * @note
 * \p num_keymaps should be at least 1; otherwise, undefined behavior will result.
 *
 * @param xPos Starting x-position of the cursor
 * @param yPos y-position of the cursor
 * @param visible_input_width Width of visible input (in pixels)
 * @param num_keymaps Number of keymaps
 * @param malloc_routine Malloc implementation to use
 * @return Pointer to IDS, if successful; \c NULL, otherwise
 */
uint24_t *textio_AllocIDS(size_t size, uint24_t xPos, uint8_t yPos, uint24_t visible_input_width, uint8_t num_keymaps, void *(malloc_routine)(size_t));

/**
 * Allocates memory for a \i Input \i Data \i Structure \i (IDS).
 *
 * @warning
 * If used outside of a function body, the memory will be allocated in the
 * global unitialized data segment (BSS). If used inside a function body, the
 * memory will be allocated on the stack. If the sprite is sufficiently large,
 * usage inside a function body will overflow the stack, so it is recommended
 * that this normally be used outside of a function body.
 *
 * @note
 * \c num_keymaps should be at least 1, otherwise undefined behavior will result.
 *
 * @param xPos Starting x-position of the cursor
 * @param yPos y-position of the cursor
 * @param visible_input_width Width of visible input (in pixels)
 * @param num_keymaps Number of keymaps
 * @return Pointer to IDS, if successful; \c NULL, otherwise
 */
#define textio_CreateIDS(size, xPos, yPos, visible_input_width, num_keymaps) \
textio_AllocIDS(size, xPos, yPos, visible_input_width, num_keymaps, malloc)

/**
 * Frees the memory allocated for the specified \p IDS.
 *
 * @note
 * Unless a different free-ing routine other than \c free()
 * needs to be used, textio_DeleteIDS should be used.
 *
 * @param IDS Pointer to IDS
 * @param free_routine Free implementation to use
 */
void textio_FreeIDS(uint24_t *IDS, void (free_routine)(void *));

/**
 * Frees the memory allocated for the specified \p IDS.
 *
 * @param IDS Pointer to IDS
 */
#define textio_DeleteIDS(IDS) \
textio_FreeIDS(IDS, free)

/**
 * Gets the size of the data buffer in the specified \p IDS.
 *
 * @param IDS Pointer to IDS
 * @return Size of data buffer
 */
uint24_t textio_GetDataBufferSize(uint24_t *IDS);

/**
 * Gets a pointer to the data buffer in the specified \p IDS.
 *
 * @param IDS Pointer to IDS
 * @return Pointer to data buffer
 */
char *textio_GetDataBufferPtr(uint24_t *IDS);

/**
 * Gets a pointer to the last character that the input function processed in
 * the specified \p IDS.
 * @see textio_Input
 *
 * @param IDS Pointer to IDS
 * @return Pointer to last character processed
 */
uint24_t textio_GetCurrCharPtr(uint24_t *IDS);

/**
 * Gets the x-position for the specified \p IDS
 *
 * @param IDS Pointer to IDS
 * @return IDS x-position
 */
uint24_t textio_GetIDSX(uint24_t *IDS);

/**
 * Gets the y-position for the specified \p IDS
 *
 * @param IDS Pointer to IDS
 * @return IDS y-position
 */
uint8_t textio_GetIDSY(uint24_t *IDS);

/**
 * Sets the cursor color for the specified \p IDS
 *
 * @param IDS Pointer to IDS
 * @param color 8bpp color
 */
void textio_SetCursorColor(uint24_t *IDS, uint8_t color);

/**
 * Gets the cursor color for the specified \p IDS
 *
 * @param IDS Pointer to IDS
 * @return Cursor color
 */
uint8_t textio_GetCursorColor(uint24_t *IDS);

/**
 * Gets the background color of the cursor.
 *
 * @note The color returned is the background color of the cursor from the last IDS used in
 * textio_Input or textio_TimedInput.
 *
 * @param IDS Pointer to IDS
 * @return Cursor's background color
*/
uint8_t textio_GetCursorBGColor(void);

/**
 * Sets the cursor blink rate. Defaults to 16.
 *
 * @note The greater the number's magnitude the slower the cursor will blink.
 * However, a blink rate of 0 will turn off cursor blinking.
 *
 * @note This function is not IDS-specific. The cursor blink rate will be the same for all IDSes
 * until it is changed by the programmer.
 *
 * @param rate A number from 0 to 255
*/
void textio_SetCursorBlinkRate(uint8_t rate);

/**
 * Gets the current cursor x-position.
 *
 * @return Current cursor x-position
*/
uint24_t textio_GetCurrCursorX(void);

/**
 * Sets the cursor y-position for the specified IDS.
 *
 * @param IDS Pointer to IDS
 * @param yPos Cursor y-position
*/
void textio_SetCursorY(uint24_t *IDS, uint8_t yPos);

/**
 * Gets the cursor y-position for the specified IDS.
 *
 * @param IDS Pointer to IDS
 * @return Cursor y-position
*/
uint8_t textio_GetCursorY(uint24_t *IDS);

/**
 * Sets the cursor width and height.
 *
 * @param IDS Pointer to IDS
 * @param width Width of cursor
 * @param height Height of cursor
 * @return True if dimensions were set; False, otherwise
 */
bool textio_SetCursorDimensions(uint24_t *IDS, uint8_t width, uint8_t height);

/**
 * Gets the cursor's width.
 *
 * @param IDS Pointer to IDS
 * @return Cursor width
 */
uint8_t textio_GetCursorWidth(uint24_t *IDS);

/**
 * Gets the cursor's height.
 *
 * @param IDS Pointer to IDS
 * @return Cursor height
 */
uint8_t textio_GetCursorHeight(uint24_t *IDS);

/**
 * Sets the amount of time in which textio_TimedInput will accept input.
 *
 * @param IDS Pointer to IDS
 * @param seconds Number of seconds
*/
void textio_SetIDSTimer(uint24_t *IDS, uint24_t seconds);

/**
 * Gets the timer value for the specified IDS.
 *
 * @note You can determine if the timer for the textio_TimedInput function
 * has expired by checking the value that this function returns.
 *
 * @param IDS Pointer to IDS
 * @return Amount of time left
*/
uint24_t textio_GetIDSTimer(uint24_t *IDS);

/**
 * Sets the state of the \c PRGM_NAME flag.
 *
 * @note This flag determines if the textio_Input will accept a number as
 * the first character of input.
 *
 * @param IDS Pointer to IDS
 * @param state State of flag
*/
void textio_SetPrgmNameFlag(uint24_t *IDS, bool state);

/**
 * Sets the state of the \c IDS_LOCK flag. When this flag is on, the input functions
 * will not accept input for the specified IDS.
 *
 * @param IDS Pointer to IDS
 * @param state State of flag
 */
void textio_LockIDS(uint24_t *IDS, bool state);

/**
 * Gets the state of the \c IDS_LOCK flag.
 *
 * @param IDS Pointer to IDS
 * @return State of flag
 */
bool textio_GetIDSLock(uint24_t *IDS);

/**
 * Gets the state of the \c BUFFER_FULL flag. This flag is set if the data buffer of the specified IDS
 * is full. The flag is reset, otherwise.
 *
 * @param IDS Pointer to IDS
 * @return State of flag
*/
bool textio_GetBufferFullFlag(uint24_t *IDS);

/**
 * Clears the data buffer of the specified IDS (sets contents to \c NULL).
 *
 * @param IDS Pointer to IDS
*/
void textio_ClearDataBuffer(uint24_t *IDS);

/**
 * Allocates memory for a new keymap.
 *
 * @note
 * Unless a different memory allocation routine other than \c malloc()
 * needs to be used, textio_CreateKeymap should be used.
 *
 * @param indicator Symbol that represents the keymap
 * @param keymap_data 50-byte character array
 * @param malloc_routine Malloc implementation to use
 * @return Pointer to keymap, if successful; \c NULL, otherwise
 */
char *textio_AllocKeymap(uint8_t indicator, const char *keymap_data, void *(malloc_routine)(size_t));

/**
 * Allocates memory for a new keymap.
 *
 * @note The \p keymap_data is the same string format used in the os_GetCSC example contained in
 * the *tice.h* documentation. E.G. \c \0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0
 *
 * @param indicator Symbol that represents the keymap
 * @param keymap_data 50-byte character array
 * @return Pointer to keymap, if successful; \c NULL, otherwise
 */
#define textio_CreateKeymap(indicator, keymap_data) \
textio_AllocKeymap(indicator, keymap_data, malloc)

/**
 * Frees the memory allocated for the specified keymap.
 *
 * @note
 * Unless a different free-ing routine other than \c free()
 * needs to be used, textio_DeleteKeymap should be used.
 *
 * @param IDS Pointer to IDS
 * @param free_routine Free implementation to use
 */
void textio_FreeKeymap(uint24_t *IDS, void (free_routine)(void *));

/**
 * Frees the memory allocated for the specified keymap.
 *
 * @param IDS Pointer to IDS
*/
#define textio_DeleteKeymap(IDS) \
textio_FreeKeymap(IDS, free)

/**
 * Sets the keymaps for the specified IDS. These keymaps determine what kind of data the
 * data buffer will hold.
 *
 * @param IDS Pointer to IDS
 * @param num_keymaps Number of keymaps
 * @param keymap0 Pointer to first keymap
 */
void textio_SetKeymaps(uint24_t *IDS, uint8_t num_keymaps, char *keymap0, ...);

/**
 * Gets the number of available keymaps for the specified IDS.
 *
 * @note The number returned will be one less than the actual number of keymaps
 * in the IDS (see introductory documentation).
 *
 * @param IDS Pointer to IDS
 * @return Number of keymaps
*/
uint8_t textio_GetNumKeymaps(uint24_t *IDS);

/**
 * Sets the initial keymap for textio_Input. The first keymap stored to the IDS
 * with textio_SetKeymaps has an index of 0; the second has an index of 1, and so on.
 *
 * @param IDS Pointer to IDS
 * @param keymap_num Number of keymap
 */
void textio_SetCurrKeymapNum(uint24_t *IDS, uint8_t number);

/**
 * Gets the current keymap number for the specified IDS.
 *
 * @param IDS Pointer to IDS
 * @return Current keymap number
*/
uint8_t textio_GetCurrKeymapNum(uint24_t *IDS);

/**
 * Gets the current keymap for the specified IDS.
 *
 * @param IDS Pointer to IDS
 */
uint24_t *textio_GetCurrKeymapPtr(uint24_t *IDS);

/**
 * Gets the codepoint that acts as the current keymap's indicator.
 *
 * @param IDS Pointer to IDS
 * @return Current keymap indicator
*/
char textio_GetCurrKeymapIndicator(uint24_t *IDS);

/**
 * Gets a pointer to the built-in TextIOC uppercase letters keymap.
 *
 * @return Pointer to keymap
*/
char *textio_GetUppercaseLettersKeymap(void);

/**
 * Gets a pointer to the built-in TextIOC lowercase letters keymap.
 *
 * @return Pointer to keymap
*/
char *textio_GetLowercaseLettersKeymap(void);

/**
 * Gets a pointer to the built-in TextIOC program_name letters keymap.
 *
 * @return Pointer to keymap
*/
char *textio_GetPrgmNameLettersKeymap(void);

/**
 * Gets a pointer to the built-in TextIOC numerical keymap.
 *
 * @return Pointer to keymap
*/
char *textio_GetNumericalKeymap(void);

/**
 * Creates an IDS and sets it up to use the TextIOC uppercase and lowercase letters keymaps.
 *
 * @note If the IDS memory allocation fails, \p name will be equal to \c NULL.
 *
 * @param name Pointer to the new IDS
 * @param xPos Starting x-position of the cursor
 * @param yPos y-position of the cursor
 * @param visible_width Width of visible input (in pixels)
*/
#define textio_CreateAlphaIDS(name, size, xPos, yPos, visible_buffer_width) \
do { name = textio_CreateIDS(size, xPos, yPos, visible_buffer_width, 2); \
if (name != NULL) { \
textio_SetKeymaps(name, 2, textio_GetUppercaseLettersKeymap(), textio_GetLowercaseLettersKeymap()); \
}; } while (0)

/**
 * Creates an IDS and sets it up to use the TextIOC numericl keymap.
 *
 * @note If the IDS memory allocation fails, \p name will be equal to \c NULL.
 *
 * @param name Pointer to the new IDS
 * @param xPos Starting x-position of the cursor
 * @param yPos y-position of the cursor
 * @param visible_width Width of visible input (in pixels)
*/
#define textio_CreateNumericalIDS(name, size, xPos, yPos, visible_buffer_width) \
do { name = textio_CreateIDS(size, xPos, yPos, visible_buffer_width, 1); \
if (name != NULL) { \
textio_SetKeymaps(name, 1, textio_GetNumericalKeymap()); \
}; } while (0)

/**
 * Creates an IDS and sets it up to use the TextIOC uppercase and lowercase letters keymaps as well
 * as the TextIOC numerical keymap.
 *
 * @note If the IDS memory allocation fails, \p name will be equal to \c NULL.
 *
 * @param name Pointer to the new IDS
 * @param xPos Starting x-position of the cursor
 * @param yPos y-position of the cursor
 * @param visible_width Width of visible input (in pixels)
*/
#define textio_CreateAlphaNumericalIDS(name, size, xPos, yPos, visible_buffer_width) \
do { name = textio_CreateIDS(size, xPos, yPos, visible_buffer_width, 3); \
if (name != NULL) { \
textio_SetKeymaps(name, 3, textio_GetUppercaseLettersKeymap(), textio_GetLowercaseLettersKeymap(), textio_GetNumericalKeymap()); \
}; } while (0)

/**
 * Creates an IDS and sets it up to use the TextIOC program_name letters keymap as well
 * as the TextIOC numerical keymap.
 *
 * @note If the IDS memory allocation fails, \p name will be equal to \c NULL.
 *
 * @param name Pointer to the new IDS
 * @param xPos Starting x-position of the cursor
 * @param yPos y-position of the cursor
 * @param visible_width Width of visible input (in pixels)
*/
#define textio_CreatePrgmNameIDS(name, size, xPos, yPos, visible_buffer_width) \
do { name = textio_CreateIDS(size, xPos, yPos, visible_buffer_width, 2); \
if (name != NULL) { \
textio_SetKeymaps(name, 2, textio_GetPrgmNameLettersKeymap(), textio_GetNumericalKeymap()); \
textio_SetPrgmNameFlag(ids, true); \
}; } while (0)

/**
 * Sets the key that activates the input function's clear action. Defaults to [clear].
 *
 * @note This function is not IDS-specific. The action will remain linked to the given key for all IDSes
 * until it is changed by the programmer.
 *
 * @param key Key
*/
void textio_SetClearKey(sk_key_t key);

/**
 * Sets the key that activates the input function's backspace action. Defaults to [del].
 *
 * @note This function is not IDS-specific. The action will remain linked to the given key for all IDSes
 * until it is changed by the programmer.
 *
 * @param key Key
*/
void textio_SetBackspaceKey(sk_key_t key);

/**
 * Sets the key that moves the cursor left. Defaults to the left arrow key.
 *
 * @note This function is not IDS-specific. The action will remain linked to the given key for all IDSes
 * until it is changed by the programmer.
 *
 * @param key Key
*/
void textio_SetCursorLeftKey(sk_key_t key);

/**
 * Sets the key that moves the cursor right. Defaults to the right arrow key.
 *
 * @note This function is not IDS-specific. The action will remain linked to the given key for all IDSes
 * until it is changed by the programmer.
 *
 * @param key Key
*/
void textio_SetCursorRightKey(sk_key_t key);

/**
 * Gets input from user and stores it in the data buffer of the specified IDS. The user is only
 * allowed to give input for the amount of time assigned to the IDS by textio_SetIDSTimer.
 *
 * @see textio_SetIDSTimer
 *
 * @param IDS Pointer to IDS
 * @return Code of key pressed
*/
sk_key_t textio_TimedInput(uint24_t *IDS);

/**
 * Gets input from user and stores it in the data buffer of the specified IDS.
 *
 * @param IDS Pointer to IDS
 * @return Code of key pressed
*/
sk_key_t textio_Input(uint24_t *IDS);

/**
 * Sets the codepoint that will stand for the theta character for the current font.
 *
 * @note This codepoint will only be valid for the font that was being used when this function
 * was called.
 *
 * @param codepoint The codepoint for the theta character
*/
void textio_SetThetaCodepoint(uint8_t codepoint);

/**
 * Gets the current codepoint for the theta character.
 *
 * @return Codepoint for theta character
*/
char textio_GetThetaCodepoint(void);

/**
 * Sets the function that TextIOC will use to draw the theta character. This is a necessary
 * function for getting program/appvar names.
 *
 * @see textio_CreatePrgmNameIDS
 *
 * @param function_ptr Pointer to function
*/
void textio_SetDrawThetaCharFunction(void *function_ptr);

/**
 * Converts any codepoints in \p name that correspond to the codepoint set by
 * textio_SetThetaCodepoint into the TI-OS theta codepoint (0x5B).
 *
 * @param name Pointer to the name of the program/appvar to convert
 * @return Pointer to the converted name
*/
char *textio_ConvertProgramAppvarName_TIOS(char *name);

/**
 * Converts any codepoints in \p name that correspond to the TI-OS theta codepoint into the
 * codepoint set by textio_SetThetaCodepoint.
 *
 * @param name Pointer to the name of the program/appvar to convert
 * @return Pointer to the converted name
*/
char *textio_ConvertProgramAppvarName_TextIOC(char *name);


/**
 * Print formatting options for textio_PrintText.
 *
 * @see textio_SetPrintFormat
*/
typedef enum {
	
	/**
	 * Enables left-margin-flush printing.
	*/
	TEXTIOC_PRINT_LEFT_MARGIN_FLUSH = 0x01,
	
	/**
	 * Enables centered printing.
	*/
	TEXTIOC_PRINT_CENTERED = 0x02,
	
	/**
	 * Enables right-margin-flush printing.
	*/
	TEXTIOC_PRINT_RIGHT_MARGIN_FLUSH = 0x03
	
} textio_print_format_options_t;

/**
 * Sets the text window that textio_PrintText will use.
 *
 * @param xPos Window x-position
 * @param yPos Window y-position
 * @param width Window width
 * @param height Window height
*/
void textio_SetTextWindow(uint24_t xPos, uint8_t yPos, uint24_t width, uint8_t height);

/**
 * Gets the text window's x-position.
 *
 * @return Window's x-position
*/
uint24_t textio_GetTextWindowX(void);

/**
 * Gets the text window's y-position.
 *
 * @return Window's y-position
*/
uint8_t textio_GetTextWindowY(void);

/**
 * Gets the text window's width.
 *
 * @return Window's width
*/
uint24_t textio_GetTextWindowWidth(void);

/**
 * Gets the text window's height.
 *
 * @return Window's height
*/
uint8_t textio_GetTextWindowHeight(void);

/**
 * Sets the amount of space (in pixels) between each line.
 *
 * @param above Amount of space above the line
 * @param below Amount of space below the line
*/
void textio_SetLineSpacing(uint8_t above, uint8_t below);

/**
 * Gets the amount of space (in pixels) between one line and the one above.
 *
 * @return Space above the line
*/
uint8_t textio_GetLineSpacingAbove(void);

/**
 * Gets the amount of space (in pixels) between one line and the one below.
 *
 * @return Space below the line
*/
uint8_t textio_GetLineSpacingBelow(void);

/**
 * Instructs TextIOC what height the current font is.
 *
 * @note This is necessary because GraphX does not provide a function that
 * returns the font height. The programmer is expected to know or calculate
 * the height of the GraphX font.
 *
 * @param height Height of current font
*/
void textio_SetFontHeight(uint8_t height);

/**
 * Sets the print format for textio_PrintText.
 *
 * @see textio_print_format_options_t
 *
 * @param format Format option code
*/
bool textio_SetPrintFormat(uint8_t format);

/**
 * Gets the current print format.
 *
 * @return Current print format code
*/
uint8_t textio_GetPrintFormat(void);

/**
 * Sets the codepoint that will act as the newline character.
 *
 * @param codepoint Codepoint
*/
void textio_SetNewlineCode(char codepoint);

/**
 * Gets the codepoint that is acting as the newline character.
 *
 * @return Codepoint
*/
char textio_GetNewlineCode(void);

/**
 * Sets the tab size.
 *
 * @note The tab size will be multiplied by the width of the current font's space to create the
 * actual pixel width of the tab.
 *
 * @param size Size of tab
*/
void textio_SetTabSize(uint8_t size);

/**
 * Gets the current tab size.
 *
 * @note This function returns the tab size, not the tab's actual pixel width.
 *
 * @return Current tab size
*/
uint8_t textio_GetTabSize(void);

/**
 * Prints a tab at the current cursor position.
*/
void textio_PrintTab(void);

/**
 * Prints a character (including tabs) at the current cursor position.
 *
 * @param character Character to print
*/
void textio_PrintChar(char character);

/**
 * Gets the width of the supplied character. This function also works for tabs, and it returns the
 * tab's actual pixel width.
 *
 * @param character Target character
 * @return Width of character
*/
uint24_t textio_GetCharWidth(char character);

/**
 * Gets the width of the supplied line.
 *
 * @param line Pointer to line
 * @param eol Pointer to the End-Of-Line
 * @return Width of line
*/
uint24_t textio_GetLineWidth(char *line, char *eol);

/**
 * Gets the width of \p num_chars characters in \c string.
 *
 * @param string Pointer to string
 * @param num_chars Number of characters
 * @return Width of the desired number of characters
*/
uint24_t textio_GetStringWidthL(char *string, uint24_t num_chars);

/**
 * Prints the supplied text in the current text window using the print format set by textio_SetPrintFormat.
 *
 * @param text Pointer to text
 * @param yPos y-position at which to start printing
*/
void textio_PrintText(char *text, uint8_t yPos);

/**
 * Returns a pointer to the \p line_num line in \p text.
 *
 * @param text Pointer to text
 * @param line_num Number of line
 * @return Pointer to desired line
*/
char *textio_GetLinePtr(char *text, uint8_t line_num);


#ifdef __cplusplus
}
#endif

#endif