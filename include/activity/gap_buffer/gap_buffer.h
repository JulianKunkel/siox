/*!
 *  @file gap_buffer.h
 * 
 *  @Author: Marc Wiedemann - based on the version of Hsin Tsao
 *  @Version: 1.2 Unicode
 *  @year 2013
 *
 *  Implementation is in /Komponenten/tools/gap-buffer as cpp file
 *
 *  A text buffer class using the buffer-gap technique for managing
 *  the text stored in the buffer with five pointers.
 *
 *  Portions of this work derived from Joseph Allen's usenet
 *  postings on comp.editor that was released to the public
 *  domain.
 *
 *  There are no restrictions on the use of this code other
 *  than to include my name in any derived work.  There are
 *  no warranty for this obviously, but you are welcomed
 *  to modify, correct, or adapt the code to your needs.  The
 *  author appreciates if you are willing to submit corrections
 *  or suggestions for improvement.
 *
 */

#include <stdio.h>

class GapBuffer {

    wchar_t *point;                    // location pointer into buffer
    wchar_t *buffer;                   // start of text buffer
    wchar_t *bufend;                   // first location outside buffer
    wchar_t *gapstart;                 // start of gap
    wchar_t *gapend;                   // first location after end of gap
    
    unsigned int GAP_SIZE;          // expand GAP by this value

    int InitBffr(unsigned int size);

    /*
     * Copy the characters from one location to another.  We have
     * to write our own instead of using memcopy because we are
     * working within a single linear buffer and thus can have
     * overlap between the source and destination.
     */
    int CopyBytes(wchar_t *destination, wchar_t *source, unsigned int length);

    /*
     *  Expand the size of the buffer.
     */
    void ExpandBuffer(unsigned int size);

    /*
     *  Expand the size of the gap.
     */
    void ExpandGap(unsigned int size);
    
public:

    static const int DEFAULT_GAP_SIZE=20;

    /* Constructor with default gap size. */
    GapBuffer(int gsize=DEFAULT_GAP_SIZE);

    /* Constructor with instantiating with an existing file. */
    GapBuffer(FILE *file, int gsize=DEFAULT_GAP_SIZE);

    /* Copy constructor to deal with our pointer members. */
    GapBuffer(const GapBuffer& tb);

    ~GapBuffer();    

    /*
     *  Returns the size of the buffer minus the gap.
     */
    int BufferSize();

    /*
     *  Move the gap to the current position of the point.
     */
    void MoveGapToPoint();

    /*
     *  Set point to offset from start of buffer.
     */
    void SetPoint(unsigned int offset);

    /*
     *  Returns the current size of the gap.
     */
    int SizeOfGap();    

    /*
     *  Returns offset from point to start of buffer.
     */
    unsigned int PointOffset();

    /*
     * Return character that point is pointing to.
     * If point is inside the gap, then return the
     * the first character outside the gap.
     */
    wchar_t GetChar();

    /*
     * Return the previous character and
     * move point back one position.
     */
    wchar_t PreviousChar();

    /*
     *  Replace the character of point. Does
     *  not move the gap.
     */
    void ReplaceChar(wchar_t ch);

    /*
     *  Get the next character and increment point.
     */
    wchar_t NextChar();

    /*
     *  Inserts a character at point location
     *  and advance the point.
     */
    void PutChar(wchar_t ch);

    /*
     *  Insert character at point position, but
     *  does NOT advance the point.
     */
    void InsertChar(wchar_t ch);

    /*
     *  Delete "size" number of characters.
     */
    void DeleteChars(unsigned int size);

    /*
     *  Inserts a length size string 
     *  at point.
     */
    void InsertString(wchar_t *string, unsigned int length);

    /*
     *  Prints out the current buffer from start
     *  to end.
     */
    void PrintBffr();

    /*
     * Saves to file the number of bytes starting from
     * the point.
     */
    int SaveBufferToFile(FILE *file, unsigned int bytes);

    
};
