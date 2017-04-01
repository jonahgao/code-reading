## compaction

### 触发compaction的条件
- 有冻结的memtable
- 某层文件个数太多（L0）或总文件大小太大（L0以上）     
  这个是level级别的compact   
  当每次有版本变更时会计算      
  相关参数：      
  > VersionSet中：    
  > - `compaction_score_`
  > - `compaction_level_`
- seek    
  这个是文件级别的compact    
  Get操作时更新、db Iterator迭代时采样    
  相关参数：
  > VersionSet中：    
  > - `file_to_compact_`
  > - `file_to_compact_level_`    
  > 
  > FileMetaData中：
  > - allowed_seeks

### 切换output 写入新文件的时机
- 当前文件的大小达到了max_file_size
- 或者当前文件与L+2层重叠的字节数太多