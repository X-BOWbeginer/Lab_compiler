#c-- compiler.lab5
######201220167 陈毅琦
###基础功能
实现了常量折叠（为了处理形如V1+#1（样例），实现了一点点替换）
####整体结构
尝试后发现，个人水平难以按照手册完整的做完实验，于是便采用了这样的方式
```c
void optimize_code(char *input[], int line_count)
{
    int has_optimized = 1;

    while (has_optimized)
    {
        has_optimized = 0;

        for (int i = 0; i < line_count; i++)
        {
            char *line = input[i];

            // 常量折叠
           //...
          // 常量传播
        }
    }
}
```
当有优化时将has_optimized置1，不断循环直到没有优化

#### 常量折叠


处理所有两个立即数的操作

#### 替换

对var：=#const，将值为常量的var进行替换，从下一行开始，直到下一次var：=的出现，服务于常量折叠


###如何编译
make即可
###值得一提的内容
无
