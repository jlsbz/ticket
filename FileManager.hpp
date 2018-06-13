#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <cstring>
#include "BPlusTree.hpp"
#include "constant.h"
#include "Node.hpp"
#include "utility.hpp"
#include "string.h"

#define start 4096

/**
 *
 * isLeaf, key number, val number, child number, then vector values sequentially.
 * finally is one addType, means the next node.
 */
namespace sjtu {

    template<class Key_Type,
            class Value_Type,
            class Compare = std::less<Key_Type>,
            class Compare_child = std::less<addType>
    >
    class FileManager {
    private:
        typedef TreeNode<Key_Type, Value_Type> Node;

        char filename[100];
        FILE *file;
        bool isOpened;

        bool Cmp(const Key_Type &x, const Key_Type &y) const {       /// <
            static Compare _cmp;
            return _cmp(x, y);
        }

        bool Equal(const Key_Type &x, const Key_Type &y) const {     /// ==
            if ((Cmp(x, y) || Cmp(y, x))) return 0;
            else return 1;
        }

    public:
        addType root_off;
        addType end_off;

        struct begin_Block{
            addType root_off = -1;
            addType end_off = start;

        };

        struct one_Block{
            char mem[4096];
        };

    private:
        void createFile() {
            file = fopen(filename, "wb");
            root_off = -1;
            end_off = start;
            begin_Block beg;

            fwrite(&beg,sizeof(begin_Block),1,file);
            fclose(file);
        }

        void init() {
            file = fopen(filename, "r");
            if (file == nullptr) {
                createFile();
                file = fopen(filename, "r+b");
            } else {
                file = fopen(filename, "r+b");
                begin_Block beg;
                fread(&beg,sizeof(begin_Block),1,file);
                root_off = beg.root_off;
                end_off = beg.end_off;

            }
        }

    public:
        FileManager() {
            filename[0] = '\0';
            isOpened = false;
            root_off  = -1;
            end_off = start;
            file = nullptr;
        }

        ~FileManager() {
            if (isOpened) {
                close_file();
            }
        }

        void set_fileName(const char *fname) {
            strcpy(filename, fname);
        }

        void clear_fileName() {
            filename[0] = '\0';
        }

        bool open_file() {

            if (isOpened) {
                return false;
            } else {
                init();
                isOpened = true;
                return true;
            }
        }

        bool close_file() {
            if (!isOpened) {
                return false;
            } else {
                fseek(file, 0, SEEK_SET);

                begin_Block beg;
                beg.root_off = root_off;
                beg.end_off = end_off;
                fwrite(&beg,sizeof(begin_Block),1,file);

                fclose(file);
                root_off  = -1;
                end_off = start;
                file = nullptr;
                isOpened = false;
                return true;
            }
        }

        bool clean() {
            if (!isOpened) return false;
            else {
                fclose(file);
                root_off = -1;
                end_off = start;
                file = fopen(filename, "w+");
                fclose(file);
                file = fopen(filename, "wb+");
                fseek(file, 0, SEEK_SET);
                begin_Block beg;
                fwrite(&beg,sizeof(begin_Block),1,file);
                return true;
            }
        }

        bool is_opened() {
            return isOpened;
        }

        void set_root(addType offset) {
            root_off = offset;
        }

        addType get_root() {
            return root_off;
        }

        bool get_next_block(const Node &cur, Node &now) {
            if (cur.next == -1)
                return false;
            else {
                get_block(cur.next, now);
                return true;
            }
        }

        bool get_root(Node &now) {
            if (root_off == -1) return false;
            else  get_block(root_off, now);
            return true;
        }

        void app_block(Node &now, bool isLeaf) {
            now.clear();
            now.address = end_off;
            now.isLeaf = isLeaf;
            end_off += BlockSize;
        }

        void get_block(addType offset, Node &now) {
            now.address = offset;
            one_Block Block;
            int pos = 0;
            fseek(file, offset, SEEK_SET);
            fread(&Block, sizeof(one_Block), 1, file);

            short K_size ,V_size, Ch_size;

            memcpy(&now.isLeaf, Block.mem + pos*sizeof(char),1);
            pos+=1;
            memcpy(&now.next, Block.mem + pos*sizeof(char), 4);
            pos+=4;
            memcpy(&K_size, Block.mem + pos*sizeof(char), 2);
            pos+=2;
            memcpy(&V_size, Block.mem + pos*sizeof(char), 2);
            pos+=2;
            memcpy(&Ch_size, Block.mem + pos*sizeof(char), 2);
            pos+=2;

            now.keys.change_len(K_size);
            now.vals.change_len(V_size);
            now.childs.change_len(Ch_size);
            memcpy(now.keys.vec, Block.mem + pos*sizeof(char),  sizeof(Key_Type)*K_size);
            pos+= sizeof(Key_Type)*K_size;
            memcpy(now.vals.vec, Block.mem + pos*sizeof(char),  sizeof(Value_Type)*V_size);
            pos+= sizeof(Value_Type)*V_size;
            memcpy(now.childs.vec, Block.mem + pos*sizeof(char),  sizeof(addType)*Ch_size);
            pos+= sizeof(addType)*Ch_size;

        }

        void write_block(Node &now) {

            one_Block Block;
            fseek(file,now.address,SEEK_SET);

            short K_size = now.keys.size();
            short V_size = now.vals.size();
            short Ch_size = now.childs.size();
            int pos = 0;
            memcpy(Block.mem + pos, &now.isLeaf, sizeof(bool));
            pos++;
            memcpy(Block.mem + pos, &now.next, 4);
            pos+=4;
            memcpy(Block.mem + pos, &K_size, 2);
            pos+=2;
            memcpy(Block.mem + pos, &V_size, 2);
            pos+=2;
            memcpy(Block.mem + pos, &Ch_size, 2);
            pos+=2;
            memcpy(Block.mem+pos, now.keys.vec, now.keys.size()*sizeof(Key_Type));
            pos+=now.keys.size()*sizeof(Key_Type);
            memcpy(Block.mem+pos, now.vals.vec, now.vals.size()*sizeof(Value_Type));
            pos+=now.vals.size()*sizeof(Value_Type);
            memcpy(Block.mem+pos, now.childs.vec, now.childs.size()*sizeof(addType));
            pos+=now.childs.size()*sizeof(addType);

             fwrite(&Block, sizeof(one_Block), 1, file);
        }


    };
};

#endif
