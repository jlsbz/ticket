#ifndef CONSTANT_H
#define CONSTANT_H



namespace sjtu {

    typedef  int  addType; /** or others  */

    const addType tree_utility_byte = 16;       ///  root_offset, head_offset, tail_offset, append_offset 4 + 4 + 4 + 4
    const addType node_utility_byte = 15;       ///  address, isLeaf, K_size, V_size, Ch_size, next. 4 + 1 + 2 + 2 + 2 + 4

    const int BlockSize = 4096;                 ///ؽ���Ż�
    
    const int TRAIN_CAPACITY = 2000;

    const int NAME_LEN = 45;
    const int DEFAULT_LEN = 25;

    const int STATION_NUM = 60;
    const int PRICE_NUM = 5;

    const int MAX_DIRECT = 150;
};

#endif
