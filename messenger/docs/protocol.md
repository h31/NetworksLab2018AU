# message format
- binary
- network byte order (big-endian)
- in & out are the same

```
<name_size : uint32>
<name string : ascii, %name_size bytes>
<msg_size : uint32>
<msg : ascii, %msg_size bytes>
```
