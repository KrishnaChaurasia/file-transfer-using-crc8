# file-transfer-using-crc8

C Implementation of data transfer using CRC-8. Transfer a file of size at least 1KB between two nodes. First, break the length into frames of size 32 bits and compute CRC bits for each frame. Secondly, augment CRC bits with frame bits for each frame and transfer the file to the receiver using Socket file transfer program (TCP file transfer). Finally, on the receiver, CRC is applied again to check whether a file is in error or not.
