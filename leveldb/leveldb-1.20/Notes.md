## compaction

### 触发compaction的条件
- 有冻结的memtable
- 某层文件个数太多（L0）或总文件大小太大（L0以上）     
  这个是level级别的compact   
  当每次有版本变更时会计算      
  相关参数：      
  > VersionSet中：    
  > - `compaction_score_`
  > - `compaction_level_`   ‘
  
  level级别的compact也是有可能分多步进行的，比如L0以外的每次L层只选一个文件   
  进度保存在`compact_pointer_`变量中，则进度会持久化到MANIFEST中
  
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
- 手动任务 CompactRange    
  手动触发compact某个user key范围。    
  执行过程是选择每层跟user key范围有重叠的文件，然后一层一层地执行。    
  而且每层可能一次compact执行不完，需要多次。    
  因为一次手动compact有总文件太小限制(`MaxFileSizeForLevel`)

### 切换output 写入新文件的时机
- 当前文件的大小达到了max_file_size
- 或者当前文件与L+2层重叠的字节数太多