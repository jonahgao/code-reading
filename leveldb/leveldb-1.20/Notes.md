## compaction

### 切换output 写入新文件的时机
- 当前文件的大小达到了max_file_size
- 或者当前文件与L+2层重叠的字节数太多