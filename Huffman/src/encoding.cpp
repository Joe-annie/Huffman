
// This is the CPP file you will edit and turn in.


#include "encoding.h"
#include "pqueue.h"
/*
#include <iostream>
#include <string>
#include "bitstream.h"
#include "HuffmanNode.h"
#include "map.h"
using namespace std;
(include in encoding.h)
also,funciton decalrations in encoding.h
*/


bool isLeaf(HuffmanNode* node);
void FindLeaves(HuffmanNode* node, Map<int, string>& map, string code = "");
int findDataInTree(ibitstream& input, HuffmanNode* encodingTree);


/*
read input from a given istream(file,string buffer..)
count and
return a map: character (represented as int here) -->#occurence in file
add a single occurrence of the fake character PSEUDO_EOF
*/
Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;
    while(true) {
        int ch = 0;
        ch = input.get(); //reads a single byte (character, 8 bits) from input; -1 at EOF
        if(ch == -1) break;
        if(freqTable.containsKey(ch)) {
//            ++ freqTable.get(ch); Not assignable
            int count = freqTable.get(ch) + 1;
            freqTable.put(ch, count);
        } else {
            freqTable.add(ch, 1);
        }
    }
    freqTable.add(PSEUDO_EOF, 1);
    return freqTable;
}
/*
 In this function you will accept a frequency table (like the one you built in buildFrequencyTable)
 and use it to create a Huffman encoding tree based on those frequencies.
 Return a pointer to the node representing the root of the tree.
 map>pq>tree

 Problem notes:
 创建node没有问题
 但是为root添加左右节点的的时候，只能添加两个节点合并成的root节点
 其他节点无法出现在树上

 修改pq的数据结构和树的生成方式
 这样就可以了
*/
HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    Vector<int> characters = freqTable.keys();
//    PriorityQueue<int> pq;
//    for(int ch : characters) {
//        pq.enqueue(ch, freqTable.get(ch));
//    }
    PriorityQueue<HuffmanNode*> pq;
    for(int ch : characters) {
        int count = freqTable.get(ch);
        pq.enqueue(new HuffmanNode(ch, count, NULL, NULL), count );
    }
    /*
     * How to Initially create a Tree?
     * Its a pointer pointing  to a ListNdoe struct
     * if there is nothing point to,it goese wrong...
    ListNode* half2 = new ListNode(front->data); //在存储空间上创建一个新节点
    ListNode* hcurr = half2;
    */
//    HuffmanNode* root = new HuffmanNode(0, 0, NULL, NULL);
//    HuffmanNode* left = new HuffmanNode(0, 0, NULL, NULL); //define in while,if not pointed to ,then inivatbly discard after the loop
//    HuffmanNode* right = new HuffmanNode(0, 0, NULL, NULL);

    while(pq.size() >= 2) { //pq.size() > 1
        //create tree node zero
//        int count = pq.peekPriority();
//        left->character = pq.dequeue();
//        left->count = count;
          HuffmanNode* left = pq.dequeue();
        //create tree node one
//            int countR = pq.peekPriority();//right-> count = freqTable(right->character); //const Map doesn't provide a call operator
//            right->character = pq.dequeue();
//            right->count = countR;
          HuffmanNode* right = pq.dequeue();
//            root->character = NOT_A_CHAR;
//            root->count = left->count + right->count;
//            root->zero = left; //看起来完全没用啊？？
//            root->one = right;
//            pq.enqueue(root->character, root->count);
          int count = left->count + right->count;
          pq.enqueue(new HuffmanNode(NOT_A_CHAR, count, left, right),count);
    }
    return pq.dequeue();
}


/*
 In this function will you accept a pointer to the root node of a Huffman tree (like the one you built in buildEncodingTree)
  and use it to create and return a Huffman encoding map:charater(int)->binary encoding(stirng)
*/
bool isLeaf(HuffmanNode* node) {
    return (node->one == nullptr && node->zero == nullptr);
}
void FindLeaves(HuffmanNode* node, Map<int, string>& map, string code) {
    //zero
    if( isLeaf(node->zero) ) {
        map.add(node->zero->character, code + "0");
    } else {
        FindLeaves(node->zero, map, code + "0");//record the path,不是leaf也要记录啊
    }
    //one
    if( isLeaf(node->one) ) {
        map.add(node->one->character, code + "1");
    } else {
        FindLeaves(node->one, map, code+ "1");
    }
}
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    // helper function for traveling the tree
    FindLeaves(encodingTree, encodingMap, "");
    return encodingMap;
}

/*
 In this function you will read one character at a time from a given input file
 use the provided encoding map to encode each character to binary
 then write the character's encoded binary bits to the given bit output bit stream,PSEUDO_EOF at end
input>map>output(bit)

 istream& ,obitstream& :The streams are already opened and ready to be read/written;
 you do not need to prompt the user or open/close the files yourself.
*/
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    while(true) {
        int ch = 0;
        ch = input.get(); //reads a single byte (character, 8 bits) from input; -1 at EOF
        if(ch == -1) break;
        //map: int(character)->huffman code,"0101"
        string binarych = encodingMap.get(ch);
        for(char c: binarych) {
            //output.writeBit(c);//Error:obitstream::writeBit: must pass an integer argument of 0 or 1. You passed the integer '1'
            //convert? '0' ascii49,'1'ascii48 --> 0,1
            output.writeBit(c - 48);
        }
    }
    //add const int PSEUDO_EOF = 256;//exist in map(add in freqTable),but not added in before loop break;
    for(char c: encodingMap.get(PSEUDO_EOF)) {
        output.writeBit(c - 48);
    }

}
/*
 Read bits from the given input file one at a time,
 and recursively walk through the specified decoding tree
 to write the original uncompressed contents of that file to the given output stream
*/
int findDataInTree(ibitstream& input, HuffmanNode* encodingTree) {
//    if(bit != -1) { not needed,cause terminated by ch
    //base case:this tree has a lonely node(the root is leaf,don't need to read bit)
    if(encodingTree->isLeaf()) {
        return encodingTree->character;
    }
    if(input.readBit() == 0) {
        return findDataInTree(input, encodingTree->zero);
    } else{
        return findDataInTree(input, encodingTree->one);
    }
}

void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
/*
    //you do not need to find actual binaary code at all,cause it stored/record in tree(in one character finding )
    // Problem notes: recursion passing parameter tree has something wrong
    //          当你完成一个字符code的时候，读下个bit是要从原来的tree root开始找的
    //          如果不用帮助函数，就会丢失，从当前完成查找的node开始
//    while(true) {
//        int bit = input.readBit();
//        if(bit == -1) break;
//        if(bit == 1) {
//            if(encodingTree->one->isLeaf()) {
//                //find one code
//                output.put(encodingTree->one->character);
//            } else {
//                decodeData(input, encodingTree->one, output);
//            }
//        } else if (bit == 0) {
//            if(encodingTree->zero->isLeaf()) {
//                output.put(encodingTree->zero->character);
//            } else {
//                decodeData(input, encodingTree->zero, output);
//            }
//        }
*/
    int ch = 0;
    while(ch != PSEUDO_EOF) {
        ch = findDataInTree(input, encodingTree); //each one starts at original root
        output.put(ch);
    }
}
/*
  Overall compress function,compress the given input file into the given output file(bitstream)
  including a header:The decoding function requires the encoding tree to be passed in as a parameter.
  writing the encodings into the compressed file, as a header.and we can generate the encoding tree from that
*/
void compress(istream& input, obitstream& output) {
    Map<int, int> freqTable = buildFrequencyTable(input);
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);
    Map<int, string> encodingMap = buildEncodingMap(encodingTre);
    output << freqTable; //Maps can easily be read and written to/from streams using << and >> operators
    encodeData(input, encodingMap,output);
    freeTree(encodingTree);// If your compress or decompress function creates a Huffman tree, that function should also free the tree.
}
/*
  it should read the bits from the given input file one at a time,
  including your header packed inside the start of the file(??
  to write the original contents of that file to the file specified by the output parameter
*/
void decompress(ibitstream& input, ostream& output) {
    Map<int, int> freqTable;
    input >> freqTable; //read the header
    HuffmanNode* encodingTree = buildEncodingMap(freqTable);
    decodeData(input,encodingTree,output);
    freeTree(encodingTree);
}
/*
 * This function should free the memory associated with the tree whose root node is represented by the given pointer.
*/
void freeTree(HuffmanNode* node) {
    //Problem:it has no effect to pass NULL,cause even null takes memory
    //so its wrong to set end contion as NULL?
//    if(node->zero != NULL) {
//        freeTree(node->zero);
//    }
//    if(node->one != NULL) {
//        freeTree(node->one);
//    }
    //change the end condition to
    if(!node) return; //是一样的吗？
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
}
