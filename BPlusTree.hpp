#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "FileManager.hpp"
#include "Node.hpp"
#include <cstring>
#include <cmath>

#include "vector.hpp"
#include "utility.hpp"

namespace sjtu
{
template<class Key_Type,
         class Value_Type,
         class Compare = std::less<Key_Type>,
         class Compare_value = std::less<Value_Type>
         >
class BPlusTree
{
    bool Cmp(const Key_Type &x, const Key_Type &y) const        ///<
    {
        static Compare _cmp;
        return _cmp(x, y);
    }

    bool Equal(const Key_Type &x, const Key_Type &y) const      /// ==
    {
        if (Cmp(x, y) || Cmp(y, x)) return 0;
        else return 1;
    }

    bool Cmp_value(const Value_Type &x, const Value_Type &y) const        ///<
    {
        static Compare_value _cmp;
        return _cmp(x, y);
    }

    bool Equal_value(const Value_Type &x, const Value_Type &y) const      /// ==
    {
        if (Cmp_value(x, y) || Cmp_value(y, x)) return 0;
        else return 1;
    }

private:
    typedef TreeNode<Key_Type, Value_Type> Node;

    struct Judge
    {
        bool success;
        bool modified;
        Judge(bool a = false, bool b = false) : success(a), modified(b) {}
    };

private:
    char filename[100];
    int branch_degree, leaf_degree;
    int half_branch_degree, half_leaf_degree;
    int K_byte, V_byte, A_byte; /**     size of   key_byte, value_byte, Addtype_byte         */

    Node pool[20];
public:

    int cnt;    /**    记录pool的num     */
    FileManager<Key_Type, Value_Type> file;

private:


    Judge erase_node(Node &now, const Key_Type &K)
    {

        if (now.isLeaf)
        {
            int del_pos = now.search_exact(K);

            if (del_pos == -1)
                return Judge(false, false);
            else
            {
                now.keys.erase(del_pos);
                now.vals.erase(del_pos);
                return Judge(true, true);
            }
        }
        else
        {
            Node &ch = pool[cnt++];
            ch.clear();
            int ch_pos = find_child(now, K, ch);
            Judge judge_info = erase_node(ch, K);
            if (!judge_info.success)
            {
                cnt--;
                return Judge(false, false);
            }
            if (ch.isLeaf)
            {
                if (ch.keys.size() >= half_leaf_degree)
                {
                    file.write_block(ch);
                    cnt--;
                    return Judge(true, false);
                }
                /**  大于一半有东西直接返回并写入 ，否则开始骚操作     */

                else
                {
                    Node &neigh = pool[cnt++];
                    int neigh_pos, key_pos;
                    Node *l_node, *r_node;
                    if (ch_pos == now.childs.size() - 1)
                    {
                        file.get_block(now.childs[now.childs.size() - 2], neigh);
                        neigh_pos = now.childs.size() - 2;
                    }
                    else
                    {
                        file.get_block(now.childs[ch_pos + 1], neigh);
                        neigh_pos = ch_pos + 1;
                    }



                    if (ch.keys.size() + neigh.keys.size() <= leaf_degree)
                    {
                        if (ch_pos < neigh_pos)
                        {
                            key_pos = ch_pos;
                            l_node = &ch;
                            r_node = &neigh;
                        }
                        else
                        {
                            key_pos = neigh_pos;
                            l_node = &neigh;
                            r_node = &ch;
                        }

                        l_node->next = r_node->next;

                        for (int i = 0; i < r_node->keys.size(); ++i)
                        {
                            l_node->keys.push_back(r_node->keys[i]);
                            l_node->vals.push_back(r_node->vals[i]);
                        }
                        now.keys.erase(key_pos);
                        now.childs.erase(key_pos + 1);
                        file.write_block(*l_node);
                        cnt -= 2;
                        return Judge(true, true);
                    }
                    else                               /**   重新分配      */
                    {
                        if (neigh_pos < ch_pos)
                        {
                            while (ch.keys.size() < half_leaf_degree)
                            {
                                Key_Type    key_shift = neigh.keys.back();
                                Value_Type  val_shift = neigh.vals.back();

                                neigh.keys.pop_back();
                                neigh.vals.pop_back();

                                ch.keys.insert(0, key_shift);
                                ch.vals.insert(0, val_shift);

                                now.keys[key_pos] = ch.keys[0];
                            }
                            file.write_block(ch);
                            file.write_block(neigh);
                            cnt -= 2;
                            return Judge(true, true);
                        }
                        else
                        {
                            while (ch.keys.size() < half_leaf_degree)
                            {
                                Key_Type    key_shift = neigh.keys.front();
                                Value_Type  val_shift = neigh.vals.front();
                                neigh.keys.erase(0);
                                neigh.vals.erase(0);
                                ch.keys.push_back(key_shift);
                                ch.vals.push_back(val_shift);
                                now.keys[key_pos] = neigh.keys[0];
                            }
                            file.write_block(ch);
                            file.write_block(neigh);
                            cnt -= 2;
                            return Judge(true, true);
                        }
                    }
                }
            }
            /**   子节点非叶子节点      */
            else
            {
                if (ch.childs.size() >= half_branch_degree)
                {
                    if (judge_info.modified)    file.write_block(ch);
                    cnt--;
                    return Judge(true, false);
                }
                else
                {
                    Node &neigh = pool[cnt++];
                    int neigh_pos;
                    int key_pos;
                    Node *l_node, *r_node;

                    if (ch_pos == now.childs.size() - 1)
                    {
                        file.get_block(now.childs[now.childs.size() - 2], neigh);
                        neigh_pos = now.childs.size() - (int) 2;
                    }
                    else
                    {
                        file.get_block(now.childs[ch_pos + 1], neigh);
                        neigh_pos = ch_pos + 1;
                    }
                    Key_Type Key = now.keys[key_pos];
                    if (ch.childs.size() + neigh.keys.size() <= branch_degree)
                    {
                        if (ch_pos < neigh_pos)
                        {
                            key_pos = ch_pos;
                            l_node = &ch;
                            r_node = &neigh;
                        }
                        else
                        {
                            key_pos = neigh_pos;
                            l_node = &neigh;
                            r_node = &ch;
                        }
                        l_node->keys.push_back(Key);
                        for (int i = 0; i < r_node->keys.size(); ++i)
                            l_node->keys.push_back(r_node->keys[i]);
                        for (int i = 0; i < r_node->childs.size(); ++i)
                            l_node->childs.push_back(r_node->childs[i]);
                        now.keys.erase(key_pos);
                        now.childs.erase(key_pos + (int) 1);
                        file.write_block(*l_node);
                        file.write_block(*r_node);
                        cnt -= 2;
                        return Judge(true, true);
                    }
                    else
                    {
                        if (neigh_pos < ch_pos)
                        {
                            while (ch.childs.size() < half_branch_degree)
                            {
                                Key_Type key_shift = neigh.keys.back();
                                addType child_shift = neigh.childs.back();
                                neigh.keys.pop_back();
                                neigh.childs.pop_back();
                                ch.keys.insert(0, Key);
                                ch.childs.insert(0, child_shift);
                                now.keys[key_pos] = key_shift;
                            }
                            file.write_block(ch);
                            file.write_block(neigh);
                            cnt -= 2;
                            return Judge(true, true);
                        }
                        else
                        {
                            while (ch.childs.size() >= half_branch_degree)
                            {
                                Key_Type key_shift = neigh.keys.front();
                                addType child_shift = neigh.childs.front();
                                neigh.keys.erase(0);
                                neigh.childs.erase(0);
                                ch.keys.push_back(key_shift);
                                ch.childs.push_back(child_shift);
                                now.keys[key_pos] = key_shift;
                            }
                            file.write_block(ch);
                            file.write_block(neigh);
                            cnt -= 2;
                            return Judge(true, true);
                        }
                    }
                }
            }
        }
    }

    Key_Type split_branch(Node &B, Node &B_next)
    {
        int mid = half_branch_degree;
        Key_Type mid_key = B.keys[mid - 1];

        B_next.childs.spilt(B.childs, mid, B.childs.size());
        B_next.keys.spilt(B.keys, mid, B.keys.size());

        B.childs.change_len(mid);
        B.keys.change_len(mid - 1);
        return mid_key;
    }

    void split_leaf(Node &L, Node &L_next)
    {

        L_next.keys.spilt(L.keys, L.keys.size() / 2, L.keys.size());
        L_next.vals.spilt(L.vals, L.vals.size() / 2, L.vals.size());
        L.keys.change_len(L.keys.size() / 2);
        L.vals.change_len(L.vals.size() / 2);
    }

    bool insert_in_leaf(Node &now, const Key_Type &K, const Value_Type &V)
    {
        int i = now.search_sup(K);
        if (i == -1)
        {
            now.keys.push_back(K);
            now.vals.push_back(V);
            return true;
        }
        now.keys.insert(i, K);
        now.vals.insert(i, V);
        return true;
    }

    int find_child(Node &now, const Key_Type K, Node &child)
    {
        int i = now.search_sup(K);
        if (i == -1)
        {
            file.get_block(now.childs.back(), child);
            return now.childs.size() - 1;
        }
        else
        {
            file.get_block(now.childs[i], child);
            return i;
        }
    }

    void search_to_leaf(Key_Type K, Node &now)
    {
        file.get_root(now);
        while (!now.isLeaf)
            find_child(now, K, now);

    }


    int find_child_multi(Node &now, const Key_Type K)
    {
        int i = now.search_sup_multi(K);
        file.get_block(now.childs[i], now);
        return i;
    }

    void search_to_leaf_multi(Key_Type K, Node &now)
    {
        file.get_root(now);

        while (!now.isLeaf)
            find_child_multi(now, K);

    }

        Judge insert_node(Node &now, const Key_Type &K, const Value_Type &V)
    {
        if (now.isLeaf)
        {
            bool suc = insert_in_leaf(now, K, V);
            return Judge(suc, suc);
        }
        else
        {
            Node &ch = pool[cnt++];
            int ch_pos = find_child(now, K, ch);
            Judge judge_info = insert_node(ch, K, V);
            if (!judge_info.success)
            {
                cnt--;
                return Judge(false, false);                     /**并不存在这种情况*/
            }
            if (ch.isLeaf)
            {
                if (ch.keys.size() > leaf_degree)
                {
                    Node &newLeaf = pool[cnt++];
                    file.app_block(newLeaf, true);

                    split_leaf(ch, newLeaf);
                    newLeaf.next = ch.next;
                    ch.next = newLeaf.address;
                    now.childs.insert(ch_pos + 1, newLeaf.address);
                    now.keys.insert(ch_pos, newLeaf.keys[0]);
                    file.write_block(ch);
                    file.write_block(newLeaf);
                    cnt -= 2;
                    return Judge(true, true);
                }
                else
                {
                    file.write_block(ch);
                    cnt--;
                    return Judge(true, false);
                }
            }
            else
            {
                if (ch.childs.size() > branch_degree)
                {
                    Node &newBranch = pool[cnt++];
                    file.app_block(newBranch, false);

                    Key_Type mid_key = split_branch(ch, newBranch);
                    now.childs.insert(ch_pos + 1, newBranch.address);
                    now.keys.insert(ch_pos, mid_key);

                    file.write_block(ch);
                    file.write_block(newBranch);
                    cnt -= 2;
                    return Judge(true, true);
                }
                else
                {
                    file.write_block(ch);
                    cnt--;
                    return Judge(true, false);
                }
            }
        }
    }













public:
    explicit BPlusTree(const char *fname)
    {
        strcpy(filename, fname);
        file.set_fileName(fname);

        K_byte = sizeof(Key_Type), V_byte = sizeof(Value_Type), A_byte = sizeof(addType);
        cnt = 0;

        leaf_degree = (BlockSize - node_utility_byte) / (K_byte + V_byte);
        branch_degree = (BlockSize - node_utility_byte) / (A_byte + K_byte);
        half_branch_degree = (int) ceil(branch_degree / 2.0);
        half_leaf_degree = (int) ceil(leaf_degree / 2.0);
    }

    ~BPlusTree()
    {
        if (file.is_opened())
            file.close_file();
    }

    bool clean()
    {
        if (!file.is_opened()) return false;
        else
        {
            for (int i = 0; i < cnt; i++)
                pool[i].clear();

            cnt = 0;
            file.clean();
            return true;
        }
    }

    bool open_file()
    {
        return file.open_file();
    }

    bool close_file()
    {
        return file.close_file();
    }

    sjtu::vector<Value_Type> find_multi(const Key_Type &K)      /**找寻 key 为 K 的所有元素*/
    {
        sjtu::vector<Value_Type> ans;
        if (!find(K).first)
            return ans;
        Node &now = pool[cnt++];
        search_to_leaf_multi(K, now);
        int i = now.search_exact(K);
        cnt--;
        if (i == -1)
        {
            if (now.next == -1)
                return ans;
            file.get_block(now.next, now);
            i = now.search_exact(K);
            if (i == -1)
                return ans;

        }
        i--;
        while (true)
        {
            i++;
            if (!Equal(now.keys[i], K)) break;
            ans.push_back(now.vals[i]);
            if (i == now.keys.size() - 1)
            {
                if (now.next == -1)
                {
                    return ans;
                }
                file.get_block(now.next, now);
                i = -1;
            }
        }
        return ans;
    }

    pair<bool, Value_Type> find(const Key_Type &K)
    {
        Node &now = pool[cnt++];
        search_to_leaf(K, now);
        int i = now.search_exact(K);
        cnt--;
        if (i == -1)
            return pair<bool, Value_Type>(false, Value_Type());
        else
            return pair<bool, Value_Type>(true, now.vals[i]);
    }

    bool modify(Key_Type K, Value_Type V)
    {
        Node &leafNode = pool[cnt++];

        search_to_leaf(K, leafNode);
        int key_pos = leafNode.search_exact(K);

        if (key_pos == -1)
        {
            cnt--;
            return false;
        }
        else
        {
            leafNode.vals[key_pos] = V;
            file.write_block(leafNode);
            cnt--;
            return true;
        }
    }

    bool insert(const Key_Type &K, const Value_Type &V)
    {
        Node &root = pool[cnt++];

        if (file.root_off == -1)
        {
            file.app_block(root, true);     ///什么都没有

            root.keys.push_back(K);
            root.vals.push_back(V);

            file.set_root(root.address);
            file.write_block(root);

            cnt--;
            return true;
        }
        file.get_root(root);

        Judge judge_info = insert_node(root, K, V);

        if (!judge_info.success)
        {
            cnt--;
            return false;                               /** 真的false就很尴尬了  ps: 实际上并不会出现false*/
        }
        /**考虑是否调整树 spilt 或者加树高*/
        else
        {
            if (root.isLeaf)
            {

                if (root.keys.size() > leaf_degree)         /**新根*/
                {
                    Node &newLeaf = pool[cnt++];
                    Node &newRoot = pool[cnt++];
                    file.app_block(newLeaf, true);

                    split_leaf(root, newLeaf);

                    newLeaf.next = root.next;
                    root.next = newLeaf.address;


                    file.app_block(newRoot, false);
                    newRoot.childs.push_back(root.address);
                    newRoot.childs.push_back(newLeaf.address);
                    newRoot.keys.push_back(newLeaf.keys[0]);
                   // file.root_off = newRoot.address;
                    file.write_root(newRoot.address);


                    file.write_block(root);
                    file.write_block(newLeaf);
                    file.write_block(newRoot);
                    cnt -= 3;
                    return true;
                }
                /**否则直接写进去就好了*/
                else
                {
                    if (judge_info.modified)
                    {
                        file.write_block(root);
                    }
                    cnt--;
                    return true;
                }
            }
            else
            {

                if (root.childs.size() > branch_degree)         /**spilt*/
                {
                    Node &newBranch = pool[cnt++];
                    Node &newRoot = pool[cnt++];
                    Key_Type mid_key;

                    file.app_block(newBranch, false);

                    mid_key = split_branch(root, newBranch);

                    file.app_block(newRoot, false);
                    newRoot.childs.push_back(root.address);
                    newRoot.childs.push_back(newBranch.address);
                    newRoot.keys.push_back(mid_key);
                    //file.root_off = newRoot.address;
                    file.write_root(newRoot.address);
                    file.write_block(root);
                    file.write_block(newBranch);
                    file.write_block(newRoot);
                    cnt -= 3;
                    return true;
                }
                else
                {
                    if (judge_info.modified)
                    {
                        file.write_block(root);
                    }
                    cnt--;
                    return true;
                }

            }
        }
    }

    bool erase(Key_Type K)
    {
        if (file.root_off == -1)
        {
            return false;
        }

        Node &root = pool[cnt++];
        Judge judge_info;
        file.get_root(root);
        judge_info = erase_node(root, K);

        if (!judge_info.success)
        {
            cnt--;
            return false;
        }
        else
        {
            if (root.isLeaf)                     /**  把b+树置为空    */
            {
                if (root.keys.size() == 0)
                {
                    //file.root_off = -1;
                    addType x = -1;
                    file.write_root(x);
                }
                if (judge_info.modified)
                    file.write_block(root);
            }
            else
            {
                if (root.childs.size() == 1)        /**   换根，减小树高*/
                {
                    //file.root_off = root.childs[0];
                    file.write_root(root.childs[0]);
                }
                else if (judge_info.modified)
                    file.write_block(root);
            }
            cnt--;
            return true;
        }
    }
};
};
#endif
