#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef int data_type;

typedef struct _treenode
{
    struct _treenode *left;//指向左子节点
    data_type data;
    struct _treenode *right;//指向右子节点
} Node;

//函数原型
/*		主要功能		*/
void bintree_auto_add( Node *root_node, int node_num );//"自动"添加节点
void bintree_print( Node *root_node, int node_num );//按序打印节点
/*		添加节点的实现细节		*/
Node *node_create( data_type data );//节点创建
void bintree_add( Node *root_node, int seq_num, data_type data );//在指定序号处添加节点
Node *parent_node_get( Node *root_node, int seq_num );//得到指定位置的父节点
/*		技术核心		*/
int node_level( int seq_num );//得到节点所在深度
int node_locate( int seq_num );//得到节点的横向位置
int *binary_locate( int locate, int level );//将横向位置转换为二进制数
/*		打印节点的实现细节		*/
Node *target_node_get( Node *root_node, int seq_num );//得到指定位置的节点
data_type *node_visit( Node *root_node, int seq_num );//访问目标节点数据域

int main( int argc, const char *argv[] )
{
	Node *root_node = node_create( 1 );//这里让根节点数据赋值为1
	
	int node_num = 15;
	bintree_auto_add( root_node, node_num );
	bintree_print( root_node, node_num );
	
	return 0;
}

//函数定义
/*		主要功能		*/
void bintree_auto_add( Node *root_node, int node_num )//"自动"添加节点
{
	for ( int i = 2; i <= node_num; ++i ){
		bintree_add( root_node, i, i );
	}
/*	这里往bintree_add()函数中传入根节点、待添加节点的序号及数据	
	i从2开始是因为i = 1表示的根节点,而根节点已经在main中创建
*/	
}

void bintree_print( Node *root_node, int node_num )//按序打印节点
{
	for ( int i = 1; i <= node_num; ++i ){
		printf("%d号结点--%d\n", i, *node_visit( root_node, i ) );
	}
/*	这里的*node_visit()效果等同于node -> data,具体的实现请见相应的函数定义	*/	
}

/*		添加节点的实现细节		*/
Node *node_create( data_type data )//节点创建
{
	Node *node = ( Node *) malloc( sizeof( Node ) );
	node -> left = node -> right = NULL;
	node -> data = data;
	return node;
}

void bintree_add( Node *root_node, int seq_num, data_type data )//在指定序号处添加节点
{
	if ( seq_num == 1 ){
		printf("根节点已存在,不可再添加根节点\n");
		return;
	}
	
	Node *parent = parent_node_get( root_node, seq_num );
	Node *node = node_create( data );
	//若序号为奇,则目标位置是其父节点的右儿子,反之左儿子
	if ( seq_num % 2 ) parent -> right = node;
	else parent -> left = node;
}

Node *parent_node_get( Node *root_node, int seq_num )//得到指定位置的父节点
{
	if ( seq_num == 1 ) return NULL;//注意序号为1,也就是根节点没有父节点
	
	int level = node_level( seq_num );
	int locate = node_locate( seq_num );
	int *arr = binary_locate( locate, level );
	
	Node *target_node = root_node, *parent_node = NULL;
	for ( int i = 0; i < level - 1; ++i ){
		//level - 1为数组长度,也就是locate - 1对应的二进制数位数
		parent_node = target_node;
		if ( arr[i] == 1 ){
			target_node = target_node -> right;
		} else {
			target_node = target_node -> left;
		}
	}
	
	free( arr ); arr = NULL;
	return parent_node;	
}

/*		技术核心		*/
int node_level( int seq_num )//得到节点所在深度
{
	int level = 0;
	for ( int i = 0; seq_num > 0; level = ++i ){
		seq_num -= pow( 2,i );
	}
	return level;	
}

int node_locate( int seq_num )//得到节点的横向位置
{
	int level = node_level( seq_num );
	return seq_num - ( ( int )pow( 2, level - 1 ) - 1);	
}

int *binary_locate( int locate, int level )//将横向位置转换为二进制数
{
	if ( level == 1 ) return NULL;
/*	注意:不适用于level = 1(即二叉树的树根)	*/
	
/*	我们的任务是把(locate - 1)转换为位数为(level - 1)的二进制数
	(可能出现前导0),并且把这个二进制数从高位到低位(从左到右)依次
	装进数组(下标从0开始)
*/	
	
	int *arr = ( int *) malloc( sizeof( int ) * ( level - 1 ) );
/*	为了简便,没有考虑用if ( a )判断内存是否分配成功
	也没有用memset( a, 0, sizeof( int ) * (level - 1) )覆盖垃圾值
*/
	//法①移位求二进制数
	int mask = 1 << ( level - 2 );
	for ( int index = 0; mask; mask >>= 1 ){
		arr[ index++ ] = mask & ( locate - 1 ) ? 1 : 0;
	}
	
	//法②数学方法
/*	将十进制转换为二进制其实就是不断用十进制数 - 2^i,
  来看十进制数有几个2(i从比较大的数开始逐步减小),如
  果结果≥0,那么对应的二进制数的第i + 1位就是1,否则0
	需要注意的是,数组下标从左到右增大,但数字的位数从左
  到右减小,两者的顺序是反的
*/	
	
//	int num = locate - 1;
//	int actual_bit = 0;//num对应的二进制数的位数
//	do {
//		++actual_bit;
//	} while ( num >= ( int )pow( 2,actual_bit ) );
/*	我测试了一下,for ( ; num >= ( int )pow( 2,actual_bit ) ; ++actual_bit ) ;
  或者把for的条件改为pow( 2,actual_bit ) - 1以及do-while最后转换的二进制数
  都是一样的
*/	
//	int total_bit = level - 1;//总共需要的二进制数位数
//	for ( int i = 0; i < total_bit - actual_bit; ++i ) arr[i] = 0;
/*	下标[0,total_bit - actual_bit - 1]装前导0,[total_bit - actual_bit,total_bit - 1]
  装二进制数
	比如9号结点,level = 4,locate = 2,那么num = locate - 1 = 1,对应的有(level - 1)位的
  二进制数为001,于是数组的[0,1]就为前导0,[2,2]为num对应的二进制数
*/
//	for ( int i = actual_bit; i > 0; --i ){
//		int index = total_bit - i;
//		int temp = ( int )pow( 2,i - 1 );
//		if ( num >= temp ){
//			num -= temp;
//			arr[ index ] = 1;
//		} else {
//			arr[ index ] = 0;
//		}
//	}
	
	return arr;	
}

/*		打印节点的实现细节		*/
Node *target_node_get( Node *root_node, int seq_num )//得到指定位置的节点
{
	//其实和parent_node_get()函数是一样的
	if ( seq_num == 1 ) return root_node;
	
	int level = node_level( seq_num );
	int locate = node_locate( seq_num );
	int *arr = binary_locate( locate, level );
	
	Node *target_node = root_node;
	for ( int i = 0; i < level - 1; ++i ){
		if ( arr[i] == 1 ){
			target_node = target_node -> right;
		} else {
			target_node = target_node -> left;
		}
	}
	
	free( arr ); arr = NULL;
	return target_node;	
}

data_type *node_visit( Node *root_node, int seq_num )//访问目标节点数据域
{
	Node *target_node = target_node_get( root_node, seq_num );
	return &( target_node -> data );
/*	这个函数的隐藏用法是通过它来修改任一节点的数据域
	比如*node_visit( root_node, 2 ) = 29;可以把2号节点的数据修改为29
		甚至可以scanf( "%d", node_visit( root_node, 2 ) );
	不过要注意scanf时不用&来获得变量的地址,因为这个函数返回的就是地址
		而printf时就要用*来访问这个地址代表的值
*/	
}
