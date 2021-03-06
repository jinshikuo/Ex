
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HASH_H_INCLUDED_
#define _NGX_HASH_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
/*
nginx的hash表示静态只读，即不能在运行时动态添加新的元素
*/


typedef struct {
    void             *value;//value,即某个key对应的值,<key, value>中的value
    u_short           len; // name的长度
    u_char            name[1];//某个要hash的数据，<key, value>中的key
} ngx_hash_elt_t; // hash元素结构


typedef struct {
    ngx_hash_elt_t  **buckets;//hash桶( 有size个桶)
    ngx_uint_t        size;// hash桶的个数
} ngx_hash_t; // hash结构


typedef struct {
    ngx_hash_t        hash;
    void             *value;
} ngx_hash_wildcard_t;

//该结构也主要用来保存hash的数据，<key, value>, 在实际使用中，一般将对个键值对保存在ngx_hash_key_t结构的数组中，
//作为参数传给ngx_hash_init()或ngx_hash_wilcard_init()函数
typedef struct {
    ngx_str_t         key;
    ngx_uint_t        key_hash;
    void             *value;
} ngx_hash_key_t;


typedef ngx_uint_t (*ngx_hash_key_pt) (u_char *data, size_t len);


typedef struct {
    ngx_hash_t            hash;
    ngx_hash_wildcard_t  *wc_head;
    ngx_hash_wildcard_t  *wc_tail;
} ngx_hash_combined_t;


typedef struct {
    ngx_hash_t       *hash;// 指向待初始化的hash结构
    ngx_hash_key_pt   key;//hash函数指针 line:42

    ngx_uint_t        max_size;//bucket的最大个数
    ngx_uint_t        bucket_size;//每个buchet的空间

    char             *name;//该hash结构的名字(在错误日志中使用)
    ngx_pool_t       *pool;
    ngx_pool_t       *temp_pool;
} ngx_hash_init_t; //hash初始化结构


#define NGX_HASH_SMALL            1
#define NGX_HASH_LARGE            2

#define NGX_HASH_LARGE_ASIZE      16384
#define NGX_HASH_LARGE_HSIZE      10007

#define NGX_HASH_WILDCARD_KEY     1
#define NGX_HASH_READONLY_KEY     2


typedef struct {
    ngx_uint_t        hsize;

    ngx_pool_t       *pool;
    ngx_pool_t       *temp_pool;

    ngx_array_t       keys;
    ngx_array_t      *keys_hash;

    ngx_array_t       dns_wc_head;
    ngx_array_t      *dns_wc_head_hash;

    ngx_array_t       dns_wc_tail;
    ngx_array_t      *dns_wc_tail_hash;
} ngx_hash_keys_arrays_t;


/*
ngx_table_elt_t是为HTTP头部"量身定制"的，其中key存储头部名称(如Content-Length)，value存储对应的值
(1024),lowcase_key是为了忽略HTTP头部名称的大小写(例如，有些客户端发来的HTTP请求头部是content-length，Nginx
希望它与大小写敏感的Content-Length做相同的处理，有了全小写的lowcase_key成员后就可以快速达成目的)
*/
typedef struct {
    ngx_uint_t        hash;//value
    ngx_str_t         key;//key
    ngx_str_t         value;
    u_char           *lowcase_key;
} ngx_table_elt_t;


void *ngx_hash_find(ngx_hash_t *hash, ngx_uint_t key, u_char *name, size_t len);
void *ngx_hash_find_wc_head(ngx_hash_wildcard_t *hwc, u_char *name, size_t len);
void *ngx_hash_find_wc_tail(ngx_hash_wildcard_t *hwc, u_char *name, size_t len);
void *ngx_hash_find_combined(ngx_hash_combined_t *hash, ngx_uint_t key,
    u_char *name, size_t len);

ngx_int_t ngx_hash_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names,
    ngx_uint_t nelts);
ngx_int_t ngx_hash_wildcard_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names,
    ngx_uint_t nelts);

#define ngx_hash(key, c)   ((ngx_uint_t) key * 31 + c)
ngx_uint_t ngx_hash_key(u_char *data, size_t len);
ngx_uint_t ngx_hash_key_lc(u_char *data, size_t len);
ngx_uint_t ngx_hash_strlow(u_char *dst, u_char *src, size_t n);


ngx_int_t ngx_hash_keys_array_init(ngx_hash_keys_arrays_t *ha, ngx_uint_t type);
ngx_int_t ngx_hash_add_key(ngx_hash_keys_arrays_t *ha, ngx_str_t *key,
    void *value, ngx_uint_t flags);


#endif /* _NGX_HASH_H_INCLUDED_ */
