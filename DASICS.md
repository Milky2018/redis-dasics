# 利用 DASICS 对漏洞进行防护

# 源码改动

## 编译环境

在编译之前，export 以下变量

```bash
export CC=riscv64-linux-gnu-gcc
export AR=riscv64-linux-gnu-ar
export RANLIB=riscv64-linux-gnu-ranlib
export STRIP=riscv64-linux-gnu-strip
```

如果用了其它工具链，需要进行相应改动。

## 编译

进入 `deps` 目录使用 `make a -j $(nproc)` 分别编译出各个依赖库。如果没有成功编译 jemalloc，可以在后续编译 Redis 的时候使用 `MALLOC=libc` 参数。

进入 `dasics/LibDASICS` 目录，使用 `make` 编译得到静态链接库。

进入 `src` 目录，使用 `make -j $(nproc)` 编译 Redis 源码，得到 redis-server, redis-cli 两个重要的二进制程序。

## 其它改动

- Redis 的 Makefile 中添加了链接选项、DASICS 的 include 目录
- 替换了使用系统调用（而非 vdso）实现的时间相关系统调用
- lua_bit 进行了 buffer 防护 

# CVE-2024-31449

## 复现方法

如果运行环境不支持网络，编写如下 conf 文件：

```
unixsocket /tmp/redis.sock
unixsocketperm 770
```

用如下命令启动 redis 服务器：

```shell
./redis-server /etc/redis.conf >/redis.log 2>&1 &
```

用如下命令启动客户端：

```shell
./redis-cli -s /tmp/redis.sock
```

然后执行如下命令：

```
eval "return bit.tohex(65535, -2147483648)" 0
```

原本应该导致服务器崩溃，现在会返回如下错误，并且服务器照常运行：

```
(error) ERR Error running script (call to f_cccc680a850df5ca0353505fdcd29abbdd0d98a9): @user_script:1: user_script:1: LUA script execution failed because of a security violation
```

## 防护说明

在 safe.c 文件中，添加了一个适用于 Redis 的统一例外处理函数 `handle_malicious_store`，它会在打印一些出错信息后，跳转到 `safe_handle_point`. 

在 lua_bit.c 文件中，添加了对 `bit.tohex` 函数的调用进行保护，关键步骤如下：

```c
static int secure_bit_tohex(lua_State *L)
{
  UBits b = barg(L, 1);
  SBits n = lua_isnone(L, 2) ? 8 : (SBits)barg(L, 2);
  char buf[8];

  int h1 = LIBCFG_ALLOC_RW(&n, sizeof(n));
  int h2 = LIBCFG_ALLOC_RW(buf, sizeof(buf));

  int val = setjmp(safe_handle_point);
  if (val == 0) {
    LIB_CALL(bit_tohex_aux, buf, b, &n);
  }

  LIBCFG_FREE(h2);
  LIBCFG_FREE(h1);
  
  if (val == 0) {
    lua_pushlstring(L, buf, (size_t)n);
    return 1;
  } else {
    return luaL_error(L, "LUA script execution failed because of a security violation");
  }
}
```

当有错误发生时，会中止 LUA 脚本的执行，并向请求返回一个错误响应；否则，正常处理请求。

# CVE-2022-0543

TODO

