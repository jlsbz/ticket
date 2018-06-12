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

        addType append_off;

        struct one{
            addType root_off = -1;

            addType append_off = start;

        };

        struct two{
            char mem[4096];
        };
        //two a;



    private:
        void createFile() {
            file = fopen(filename, "wb");
            root_off = -1;
            append_off = start;
            one b;

            fwrite(&b,sizeof(one),1,file);
            fclose(file);
        }

        void init() {
            file = fopen(filename, "r");
            if (file == nullptr) {
                createFile();
                file = fopen(filename, "r+b");
            } else {
                file = fopen(filename, "r+b");
                one b;
                fread(&b,sizeof(one),1,file);
                root_off = b.root_off;
                append_off = b.append_off;

            }
        }

    public:
        FileManager() {
            filename[0] = '\0';
            isOpened = false;
            root_off  = -1;
            append_off = start;
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
                std::cout << "false!";
                return false;
            } else {
                fseek(file, 0, SEEK_SET);
                one b;
                b.root_off = root_off;
                b.append_off = append_off;
                fwrite(&b,sizeof(one),1,file);


                fclose(file);
                root_off  = -1;
                append_off = start;
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
                append_off = start;
                file = fopen(filename, "w+");
                fclose(file);
                file = fopen(filename, "wb+");
                fseek(file, 0, SEEK_SET);
                one b;
                fwrite(&b,sizeof(one),1,file);
                return true;
            }
        }

        bool is_opened() {
            return isOpened;
        }

        void get_block(addType offset, Node &ret) {
            ret.address = offset;
            two a;
            int pos = 0;
            fseek(file, offset, SEEK_SET);
            fread(&a, sizeof(two), 1, file);

            short K_size ,V_size, Ch_size;


            memcpy(&ret.isLeaf, a.mem +pos*sizeof(char),1);
            pos+=1;
            memcpy(&ret.next, a.mem + pos*sizeof(char), 4);
            pos+=4;
            memcpy(&K_size, a.mem + pos*sizeof(char), 2);
            pos+=2;
            memcpy(&V_size, a.mem + pos*sizeof(char), 2);
            pos+=2;
            memcpy(&Ch_size, a.mem + pos*sizeof(char), 2);
            pos+=2;

            ret.keys.shorten_len(K_size);
            ret.vals.shorten_len(V_size);
            ret.childs.shorten_len(Ch_size);
            memcpy(ret.keys.vec, a.mem + pos*sizeof(char),  sizeof(Key_Type)*K_size);
            pos+= sizeof(Key_Type)*K_size;
            memcpy(ret.vals.vec, a.mem + pos*sizeof(char),  sizeof(Value_Type)*V_size);
            pos+= sizeof(Value_Type)*V_size;
            memcpy(ret.childs.vec, a.mem + pos*sizeof(char),  sizeof(addType)*Ch_size);
            pos+= sizeof(addType)*Ch_size;

        }

        bool get_next_block(const Node &cur, Node &ret) {
            if (cur.next == -1)
                return false;
            else {
                get_block(cur.next, ret);
                return true;
            }
        }

        bool get_root(Node &ret) {
            if (root_off == -1) return false;
            else  get_block(root_off, ret);
            return true;
        }


        void append_block(Node &ret, bool isLeaf) {
            ret.clear();
            ret.address = append_off;
            ret.isLeaf = isLeaf;
            append_off += BlockSize;
        }






        void write_block(Node &now) {

            two a;
            fseek(file,now.address,SEEK_SET);


            short K_size = now.keys.size();
            short V_size = now.vals.size();
            short Ch_size = now.childs.size();
            int pos = 0;
            memcpy(a.mem + pos, &now.isLeaf, sizeof(bool));
            pos++;
            memcpy(a.mem + pos, &now.next, 4);
            pos+=4;
            memcpy(a.mem + pos, &K_size, 2);
            pos+=2;
            memcpy(a.mem + pos, &V_size, 2);
            pos+=2;
            memcpy(a.mem + pos, &Ch_size, 2);
            pos+=2;
            memcpy(a.mem+pos,now.keys.vec,now.keys.size()*sizeof(Key_Type));
            pos+=now.keys.size()*sizeof(Key_Type);
            memcpy(a.mem+pos,now.vals.vec,now.vals.size()*sizeof(Value_Type));
            pos+=now.vals.size()*sizeof(Value_Type);
            memcpy(a.mem+pos,now.childs.vec,now.childs.size()*sizeof(addType));
            pos+=now.childs.size()*sizeof(addType);

             fwrite(&a, sizeof(two), 1, file);
        }

        void set_root(addType offset) {
            root_off = offset;
        }

        addType get_root() {
            return root_off;
        }




    };
};

#endif
