#include <stdio.h>
#include "json.h"

int test_create_json()
{
	/*
	{
		"semantic": 
		{
			"slots":
			{
				"name":"张三"
			}
		},
		"rc":0,
		"operation":"CALL",
		"service":"telephone",
		"text":"打电话给张三"
	}
	*/

    JsonObject* root = json_create();
    JsonObject* semantic = json_create();
    JsonObject* slots = json_create();

	//root节点下添加semantic节点
	json_add_string(root, "semantic", semantic);
	
	//semantic节点下添加slots节点
	json_add_string(semantic, "slots", slots);

    //添加name节点
	json_add_string(slots, "name", ("张三"));

	//根节点下添加
    json_add_string(root, "rc", (0));
    json_add_string(root, "operation", ("CALL"));
    json_add_string(root, "service", ("telephone"));
    json_add_string(root, "text", ("打电话给张三"));
    
    printf("%s\n", cJSON_Print(root));

	//释放内存
	json_destroy(root);

    return 0;
}


