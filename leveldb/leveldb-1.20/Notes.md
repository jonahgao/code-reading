## compaction

### 触发compaction的条件
- 有冻结的memtable
- 某层文件个数太多（L0）或总文件大小太大（L0以上）     
  每次有版本变更时计算score
- seek    
  Get操作时更新、db Iterator迭代时采样

### 切换output 写入新文件的时机
- 当前文件的大小达到了max_file_size
- 或者当前文件与L+2层重叠的字节数太多