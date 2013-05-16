#include "BipBuffer.h"
#include "HPSA.h"

BipBuffer history;
SOCKET s;

bool read_EOF;

bool StartUp
{
    // Allocate a buffer 4 Gigabytes in length
    // for dynamic allocation we could allocate a second larger one and copy it, remove the former one. This can be done any time more space is needed
    if (!history.AllocateBuffer(4194304)) return false;
    readEOF = false;

    s = socket( //    ... do something ...
		);

}

void recv_information-push_to historybuffer()
{
    _ASSERTE(history.IsValid());

    // Reserve as much space as possible in the buffer:
    int space;
    BYTE* pData = history.Reserve(GetBufferSize(), space);

    // We now have *space* amount of room to play with.

    if (pData == NULL) return;

    // Obviously we've not emptied the buffer recently
    // because there isn't any room in it if we return.

    // Use the buffer!
    int recvcount = recv(s, (char*)pData, space, 0);

    // error handling
    if (recvcount == SOCKET_ERROR) return;
    
    // We now have data in the buffer (or, if the
    // connection was gracefully closed, we don't have any)

    history.Commit(recvcount);

    if (recvcount == 0) read_EOF = true;

}

void push-information_to_base()
{
    _ASSERTE(history.IsValid());

    // Let's empty the buffer.

    int allocated;
    BYTE* pData;

    while (pData = history.GetContiguousBlock(allocated)
           != NULL)
    {
        // Let's do something with the data.

        fwrite(pData, allocated, 1, outputfile);

//	push_to_base(pData, allocated, 1, outputfile);


        // (error handling)
        history.DecommitBlock(allocated);
    }
}
