#include <string.h>
#include <string>
#include <fstream>
#include <malloc.h>
#include<iostream>
#include<sstream>
#include<windows.h>
#include "curl/curl.h"
#include "cJSON.h"
#include "Utils.h"
#include "common.h"
#include<process.h>
#include <stdio.h>
#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "oss_auth.h"
#include "oss_util.h"
#include "oss_api.h"
#include <sqlite3.h>
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<cv.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
using namespace cv;

std::vector<Rect> faces;
string face_cascade_name = "C:/Users/Zou/Desktop/AliyunAPI/haarcascade_frontalface_alt2.xml";
//该文件存在于OpenCV安装目录下的\sources\data\haarcascades内，需要将该xml文件复制到当前工程目录下
CascadeClassifier face_cascade;
void detectAndDisplay(Mat frame, int flage);
int temp1 = 1;
int temp = 1;
#define MODULE_NAME "MAIN"
#define MAX_LEN 256
char *url_str = NULL;
typedef struct
{
	char *buff;
	int  len;
}RespData;
//

void detectAndDisplay(Mat face, int flage)
{

	Mat face_gray;
	cvtColor(face, face_gray, CV_BGR2GRAY);  //rgb类型转换为灰度类型
	equalizeHist(face_gray, face_gray);   //直方图均衡化
	face_cascade.detectMultiScale(face_gray, faces, 1.1, 4, 0, Size(30, 30));//找出人脸

	for (size_t j = 0; j < faces.size(); j++)
	{
		Mat faceROI = face(faces[j]);
		Mat MyFace;
		if (faceROI.cols > 40)
		{
			resize(faceROI, MyFace, Size(112, 112));//调整图像大小为92*112
			string  str = format("C:/Users/Zou/Desktop/%d.jpg", flage);
			imwrite(str, MyFace);
			imshow("处理结果", MyFace);
		}
	}

}

unsigned int _stdcall ThreadProc(LPVOID lpParameter)
{
	std::cout << "thread function ThreadProc!\n";
	return 0;
}

char *geturl()
{
	//由于url地址是固定的阿里云的位置+bucketname+图片的名字因此可以用下面的方法来实现
	char url[80];
	strcpy(url, "https://facedatabase.oss-cn-shanghai.aliyuncs.com/data/");
	strcat(url, "1.jpg");
	//puts(url);
	char filename[255];
	int i;

	for (i = 1; i <= 2; i++) {
		sprintf(filename, "https://facedatabase.oss-cn-shanghai.aliyuncs.com/data/%d.jpg", i);
		//HttpPostaliyuntwo(url, buff, buff2, len, len2, ak_id, ak_secret);

		printf("%s\n", filename);
	}
	
	return url;
	
}

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {

	RespData *resp = (RespData*)userp;
	size_t len = size*nmemb;

	memcpy(resp->buff + resp->len, buffer, len);
	resp->len += len;

	return len;
}

//#define __DETECT__
#define __VERIFY__

#define __URL__

static int callback(void *res, int argc, char **argv, char **azColName) {
	int i;
	//fprintf(stderr, "%s: ", (const char*)data);
	/*for (i = 0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}*/
	if (argc == 1) {
		*((int*)res) = atoi(argv[0]);
	}
	printf("\n");
	return 0;
}


std::string HttpPostaliyuntwo(const std::string& url, const unsigned char *img1, const unsigned char *img2, int imagelen1, int imageLen2, const std::string& ak_id, const std::string& ak_secret, std::string image_url_1, std::string image_url_2)
{
#ifdef TEST_TIME
	long long premsec = getMillSec();
#endif
	std::string responseStr1("");         //创建一个名为 responseStr 的 string，并将 “”赋值给他
	static RespData respData = { 0, 0 };  //初始化结构体
	int size = 102400;
	if (respData.buff == 0)              //如果结构体buff里的数据为0
		respData.buff = new char[size];
	respData.len = 0;
	memset(respData.buff, 0, size);       //清空buff里面的数据
	cJSON *root = cJSON_CreateObject();
	std::string base64str = Utils::Base64Encode(img1, imagelen1); //将第一个image变成base64
	std::string base64str2 = Utils::Base64Encode(img2, imageLen2); //将第二个image变成base64

																   //人脸检测
#ifdef __DETECT__ 

																   //通过url识别
#ifdef __URL__
	std::string image_url = "https://img.alicdn.com/tfs/TB1pIGxdMoQMeJjy0FpXXcTxpXa-400-300.jpg";
	std::string body("{\"type\":0,\"image_url\":");
	body += "\"" + image_url + "\"}";	//添加图片的url
#else
																   //通过图片content识别
	std::string body("{\"type\":1,\"content\":");
	body += "\" " + base64str + "\"}";
#endif

#endif

	//人脸比对
#ifdef __VERIFY__

	//通过url识别
#ifdef __URL__
	
	//std::string image_url_1 = "https://qwq2143.oss-cn-shanghai.aliyuncs.com/test/1.jpg";
	//std::string image_url_2 = "https://qwq2143.oss-cn-shanghai.aliyuncs.com/test/3.jpg";
	std::string body("{\"type\":0,\"image_url_1\":");
	body += "\"" + image_url_1 + "\",";						//添加图片1的url
	body += "\"image_url_2\":\"" + image_url_2 + "\"}";		//添加图片2的URL
#else 
	//通过图片content识别
	std::string body("{\"type\":1,\"content_1\":");
	body += "\" " + base64str + "\",";
	body += "\"content_2\":";
	body += "\" " + base64str2 + "\"}";
#endif 

#endif

	/*
	{"type":1,"content":" base64str"}
	{"type":1, "content_1": "base64str", "content_2": " base64str2"}
	*/

	// http header 参数
	std::string method = "POST";
	std::string accept = "application/json";
	std::string content_type = "application/json";

#ifdef __DETECT__ 
	std::string path = "/face/detect";
#endif

#ifdef __VERIFY__
	std::string path = "/face/verify";
#endif

	std::string date = Utils::GetGMTDatetime();
	std::string bodyMd5str = Utils::Md5Base64(body);

	// 1.对body做MD5+BASE64加密
	std::string stringToSign = method + "\n" + accept + "\n" + bodyMd5str + "\n" + content_type + "\n" + date + "\n" + path;
	// 2.计算 HMAC-SHA1
	std::string signature = Utils::HMACSha1Base64(stringToSign, ak_secret);
	// 3.得到 authorization header
	std::string authHeader = "Dataplus " + ak_id + ":" + signature;

	hprintf("url       : %s\n", url.c_str());
	hprintf("date      : %s\n", date.c_str());
	hprintf("bodyMd5str: %s\n", bodyMd5str.c_str());
	hprintf("signature : %s\n", signature.c_str());


#ifdef TEST_TIME
	hprintf("%d bytes prepare cost:%lldms\n", imageLen, getMillSec() - premsec);
#endif
	CURL* curl = NULL;
	CURLcode res = CURLE_OK;

	struct curl_slist* headers = NULL;
	char tmp_str[256] = { 0 };

	// 设置通用的请求属性
	snprintf(tmp_str, sizeof(tmp_str), "Content-Type: %s", "application/json");
	headers = curl_slist_append(headers, tmp_str);
	snprintf(tmp_str, sizeof(tmp_str), "Accept: %s", "application/json");
	headers = curl_slist_append(headers, tmp_str);
	snprintf(tmp_str, sizeof(tmp_str), "Date: %s", date.c_str());
	headers = curl_slist_append(headers, tmp_str);
	snprintf(tmp_str, sizeof(tmp_str), "Authorization: %s", authHeader.c_str());
	headers = curl_slist_append(headers, tmp_str);

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respData);
		//printf("[%s][%d]:url.c_str()是:%s", __func__, __LINE__, url.c_str());
		res = curl_easy_perform(curl);
		hprintf("resp:\n%s\n", respData.buff);
		if (res != CURLE_OK) {                                               //错误处理
			hprintf("curl_easy_perform() ret %d: %s \n", res, curl_easy_strerror(res));
			cJSON_AddBoolToObject(root, "succ", false);
			cJSON_AddStringToObject(root, "desc", curl_easy_strerror(res));
		}
		else
		{
			cJSON *c = cJSON_Parse(respData.buff);
			if (!c)
			{
				hprintf("%s\n", "parse json failed.");
				return 0;
			}
			cJSON *error = cJSON_GetObjectItem(c, "errno");
			printf("error = %d \n", error->valueint);
			cJSON *confidence = cJSON_GetObjectItem(c, "confidence");
			printf("json的confidence是：%f\n", confidence->valuedouble);
			if (confidence->valuedouble > 60)   //如果人脸的置信度很高，则从数据库中查找数据
			{
				sqlite3 *db;
				char *zErrMsg = 0;
				int rc;
				int success;
				char *sql;
				char *sql_str;
				//std::string sql_str;
				//std::string sql_str1;
				const char* data = "Callback function called";
				/* Open database */
				rc = sqlite3_open("C:\\Users\\Zou\\Desktop\\AliyunAPI\\AliyunAPI\\test.db", &db);
				if (rc) {
					fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
					exit(0);
				}
				else {
					fprintf(stderr, "Opened database successfully\n");
				}
				/* Create SQL statement */
				//sql_str = "SELECT * from userinfo where user_id=";
				//sql_str1 = "UPDATE userinfo set is_come = 1 where user_id=";
				sql = new char[200];
				memset(sql, '\0', 200);
				sql_str = new char[100];
				memset(sql_str, '\0', 100);
				sprintf(sql, "SELECT * from userinfo where user_id=%d", temp); 
				sprintf(sql_str, "UPDATE userinfo set is_come = 1 where user_id=%d", temp);
				/* Execute SQL statement */
				cout << sql_str << endl;
				success = sqlite3_exec(db, sql_str, callback, (void*)data, &zErrMsg);
				rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
				
				if (rc != SQLITE_OK&&success!=SQLITE_OK) {
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
				else {
					fprintf(stdout, "Operation done successfully\n");
				}
				sqlite3_close(db);
				
			}
			
			cJSON *rectA = cJSON_GetObjectItem(c, "rectA");
			if (error->valueint != 0)
			{
				cJSON_AddBoolToObject(root, "succ", false);
				cJSON_AddNumberToObject(root, "error", error->valueint);
			}
			else
			{
				if (rectA->valueint == 0)
					cJSON_AddBoolToObject(root, "succ", false);
				else
				{
					cJSON_AddBoolToObject(root, "succ", true);
				}
			}

		}
		curl_easy_cleanup(curl);
	}
	curl_slist_free_all(headers);
	char *pstr = cJSON_Print(root);
	responseStr1.assign(pstr);
	hprintf("HttpPostAliYun json:\n%s\n", pstr);
	free(pstr);
	cJSON_Delete(root);
#ifdef TEST_TIME
	hprintf("HttpPostAliYun %d bytes cost:%lldms\n", imageLen, getMillSec() - premsec);
#endif
	//std::string responseStr1("");         //创建一个名为 responseStr 的 string，并将 “”赋值给他
	return responseStr1;


}
void init_options(oss_request_options_t *options) {
	options->config = oss_config_create(options->pool);
	aos_str_set(&options->config->endpoint, "oss-cn-shanghai.aliyuncs.com");
	aos_str_set(&options->config->access_key_id, "LTAIsc3yrMfBdqAQ");
	aos_str_set(&options->config->access_key_secret, "R6YFX2H17TN9QBk6ZnI8mbwYiO9i9e");
	options->config->is_cname = 0;
	options->ctl = aos_http_controller_create(options->pool, 0);
}
void delete_object()
{

	aos_pool_t *p = NULL;
	aos_string_t bucket;
	aos_string_t object;
	oss_request_options_t *options = NULL;
	aos_table_t *resp_headers = NULL;
	aos_status_t *s = NULL;
	aos_pool_create(&p, NULL);
	/* 创建并初始化options */
	options = oss_request_options_create(p);
	init_options(options);
	printf("请输入需要上传比对图片的路径");
	char *filename1;
	filename1 = (char*)malloc(sizeof(char));
	scanf("%s", filename1);
	aos_str_set(&bucket, "facedatabase");
	aos_str_set(&object, filename1);
	/* 删除文件 */
	s = oss_delete_object(options, &bucket, &object, &resp_headers);
	/* 判断是否删除成功 */
	if (aos_status_is_ok(s)) {
		printf("delete face_picture succeed\n");
	}
	else {
		printf("delete face_picture failed\n");
	}
	/* 释放资源*/
	aos_pool_destroy(p);
}

void put_object_from_file()
{
	//printf("请输入需要上传比对图片的位置");
	char *filename="C:\\Users\\Zou\\Desktop\\11.jpg";
	//filename = (char*)malloc(sizeof(char));

	//= "C:/Users/Zou/Desktop/1.jpg"
	//scanf("%s", filename);
	//printf("%s", filename);
	aos_pool_t *p = NULL;
	aos_string_t bucket;
	aos_string_t object;
	aos_table_t *headers = NULL;
	aos_table_t *resp_headers = NULL;
	oss_request_options_t *options = NULL;
	
	aos_status_t *s = NULL;
	aos_string_t file;
	aos_pool_create(&p, NULL);
	/* 创建并初始化options */
	options = oss_request_options_create(p);
	init_options(options);
	/* 初始化参数 */
	headers = aos_table_make(options->pool, 1);
	apr_table_set(headers, "image/jpeg", "image/jpeg");
	aos_str_set(&bucket, "facedatabase");
	aos_str_set(&object, "uploaddata/1.jpg");
	aos_str_set(&file, filename);
	/* 上传文件 */
	s = oss_put_object_from_file(options, &bucket, &object, &file,
		headers, &resp_headers);
	/* 判断是否上传成功 */
	if (aos_status_is_ok(s)) {
		printf("put object from file succeeded\n");
	}
	else {
		printf("put object from file failed\n");
	}
	/* 释放资源*/
	aos_pool_destroy(p);
}

void put_object_from_file_test()
{
	
	//连接数据库获取个数
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	int success;
	char *sql;
	char *sql_str;
	const char* data = "Callback function called";
	/* Open database */
	rc = sqlite3_open("C:\\Users\\Zou\\Desktop\\AliyunAPI\\AliyunAPI\\test.db", &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}
	sql = new char[200];
	memset(sql, '\0', 200);
	sql = "select count(*) from userinfo";
	char ** pResult;
	char *errmsg;
	int nRow, nCol;
	auto res = sqlite3_get_table(db, sql, &pResult, &nRow, &nCol, &errmsg);
	if (res == SQLITE_OK) {
		cout << pResult[nCol] << endl;
		int data = atoi(pResult[nCol]);
		printf("count: %d", data);
		temp1 = data;
	}
	sqlite3_close(db);
}

std::string HttpPostAliYun(const std::string& url, const unsigned char *bmpImage, int imageLen, const std::string& ak_id, const std::string& ak_secret)//第一个参数是请求的url地址
{
#ifdef TEST_TIME
	long long premsec = getMillSec();
#endif
	std::string responseStr("");
	static RespData respData = { 0, 0 };
	int size = 102400;
	if (respData.buff == 0)
		respData.buff = new char[size];
	respData.len = 0;
	memset(respData.buff, 0, size);

	cJSON *root = cJSON_CreateObject();

	std::string base64str = Utils::Base64Encode(bmpImage, imageLen);
	std::string body("{\"type\":1,\"content\":");
	body += "\"" + base64str + "\"}";

	std::string method = "POST";
	std::string accept = "application/json";
	std::string content_type = "application/json";
	std::string path = "/face/detect";
	std::string date = Utils::GetGMTDatetime();
	std::string bodyMd5str = Utils::Md5Base64(body);

	std::string stringToSign = method + "\n" + accept + "\n" + bodyMd5str + "\n" + content_type + "\n" + date + "\n" + path;
	std::string signature = Utils::HMACSha1Base64(stringToSign, ak_secret);
	std::string authHeader = "Dataplus " + ak_id + ":" + signature;
	//hprintf("body:\n%s\n", body.c_str());
	hprintf("url       : %s\n", url.c_str());
	hprintf("date      : %s\n", date.c_str());
	hprintf("bodyMd5str: %s\n", bodyMd5str.c_str());
	hprintf("signature : %s\n", signature.c_str());
#ifdef TEST_TIME
	hprintf("%d bytes prepare cost:%lldms\n", imageLen, getMillSec() - premsec);
#endif
	CURL* curl = NULL;
	CURLcode res = CURLE_OK;

	struct curl_slist* headers = NULL;
	char tmp_str[256] = { 0 };


	snprintf(tmp_str, sizeof(tmp_str), "Content-Type: %s", "application/json");
	headers = curl_slist_append(headers, tmp_str);
	snprintf(tmp_str, sizeof(tmp_str), "Accept: %s", "application/json");
	headers = curl_slist_append(headers, tmp_str);
	snprintf(tmp_str, sizeof(tmp_str), "Date: %s", date.c_str());
	headers = curl_slist_append(headers, tmp_str);
	snprintf(tmp_str, sizeof(tmp_str), "Authorization: %s", authHeader.c_str());
	headers = curl_slist_append(headers, tmp_str);

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respData);

		res = curl_easy_perform(curl);
		hprintf("resp:\n%s\n", respData.buff);
		if (res != CURLE_OK) {
			hprintf("curl_easy_perform() ret %d: %s \n", res, curl_easy_strerror(res));
			cJSON_AddBoolToObject(root, "succ", false);
			cJSON_AddStringToObject(root, "desc", curl_easy_strerror(res));
		}
		else
		{
			cJSON *c = cJSON_Parse(respData.buff);
			if (!c)
			{
				hprintf("%s\n", "parse json failed.");
				return "";
			}
			cJSON *error = cJSON_GetObjectItem(c, "errno");
			cJSON *facenum = cJSON_GetObjectItem(c, "face_num");
			if (error->valueint != 0)
			{
				cJSON_AddBoolToObject(root, "succ", false);
				cJSON_AddNumberToObject(root, "error", error->valueint);
			}
			else
			{
				if (facenum->valueint == 0)
					cJSON_AddBoolToObject(root, "succ", false);
				else
				{
					cJSON_AddBoolToObject(root, "succ", true);
					cJSON *landmarks = cJSON_GetObjectItem(c, "landmark");
					cJSON *pose = cJSON_GetObjectItem(c, "pose");
					cJSON *alignX = cJSON_GetArrayItem(landmarks, 112);
					cJSON *alignY = cJSON_GetArrayItem(landmarks, 113);
					cJSON *faceLeftX = cJSON_GetArrayItem(landmarks, 192);
					cJSON *faceLeftY = cJSON_GetArrayItem(landmarks, 193);
					cJSON *faceRightX = cJSON_GetArrayItem(landmarks, 194);
					cJSON *faceRightY = cJSON_GetArrayItem(landmarks, 195);
					cJSON *yaw = cJSON_GetArrayItem(pose, 0);
					cJSON *pitch = cJSON_GetArrayItem(pose, 1);
					cJSON *roll = cJSON_GetArrayItem(pose, 2);
					int s = (int) pow(faceRightX->valueint - faceLeftX->valueint, 2) + pow(faceRightY->valueint - faceLeftY->valueint, 2);
					int facew = (int) sqrt(s);
					cJSON_AddNumberToObject(root, "faceX", alignX->valueint);
					cJSON_AddNumberToObject(root, "faceY", alignY->valueint);
					cJSON_AddNumberToObject(root, "faceW", facew);
					cJSON_AddNumberToObject(root, "yaw", yaw->valuedouble);
					cJSON_AddNumberToObject(root, "pitch", pitch->valuedouble);
					cJSON_AddNumberToObject(root, "roll", roll->valuedouble);
				}
			}
			cJSON_Delete(c);
		}
		curl_easy_cleanup(curl);
	}

	curl_slist_free_all(headers);
	char *pstr = cJSON_Print(root);
	responseStr.assign(pstr);
	hprintf("HttpPostAliYun json:\n%s\n", pstr);
	free(pstr);
	cJSON_Delete(root);
#ifdef TEST_TIME
	hprintf("HttpPostAliYun %d bytes cost:%lldms\n", imageLen, getMillSec() - premsec);
#endif
	return responseStr;
}


int main(int argc, char *argv[])
{
	if (aos_http_io_initialize(NULL, 0) != AOSE_OK) {
		return -1;
	}        //初始化

	put_object_from_file_test();
	
	/*int grade;
	scanf("%d", &grade);
	switch (grade)
	{
	case 1:
	{*/
		int i = 11;
		//printf("请输入需要裁剪的图片序号");
		//scanf("%d", &i);
		if (!face_cascade.load(face_cascade_name)) {
			printf("级联分类器错误，可能未找到文件，拷贝该文件到工程目录下！\n");
			return -1;
		}
		string  str = format("C:/Users/Zou/Desktop/%d.jpg", i);//读取文件
		Mat image;
		image = imread(str, 1);  //当前工程的image目录下的lyf.jpg文件，注意目录符号
		if (image.empty())
		{
			printf("no find image!");
		}
		imshow("人脸识别", image);
		waitKey(1000);
		detectAndDisplay(image, i); //调用人脸检测函数
		waitKey(1000);
		if (aos_http_io_initialize(NULL, 0) != AOSE_OK) {
			return -1;
		}
		put_object_from_file(); //从本地上传文件到oss
		//system("pause");
		std::string facefile = "C:/Users/Zou/Desktop/1.jpg";
		std::string facefile2 = "C:/Users/Zou/Desktop/2.jpg";
		std::string ak_id = "LTAIsc3yrMfBdqAQ";
		std::string ak_secret = "R6YFX2H17TN9QBk6ZnI8mbwYiO9i9e";
#ifdef __DETECT__  
		std::string url = "https://dtplus-cn-shanghai.data.aliyuncs.com/face/detect";
#endif

#ifdef __VERIFY__
		std::string url = "https://dtplus-cn-shanghai.data.aliyuncs.com/face/verify";
#endif

		std::ifstream in(facefile.c_str(), std::ifstream::binary);
		std::ifstream in2(facefile2.c_str(), std::ifstream::binary);
		if (in && in2)
		{
			in.seekg(0, std::ifstream::end);
			in2.seekg(0, std::ifstream::end);
			int len = in.tellg();
			int len2 = in2.tellg();
			in.seekg(0, std::ifstream::beg);
			in2.seekg(0, std::ifstream::beg);
			unsigned char *buff = new unsigned char[len];
			unsigned char *buff2 = new unsigned char[len2];
			in.read((char*)buff, len);
			in2.read((char*)buff2, len2);
			in.close();
			in2.close();
#ifdef __DETECT__  
			HttpPostAliYun(url, buff, len, ak_id, ak_secret);
#endif
#ifdef __VERIFY__
			std::ostringstream stream;
			std::string filename1 = "https://facedatabase.oss-cn-shanghai.aliyuncs.com/uploaddata/1.jpg";
			for (temp = 1; temp <= temp1; temp++)
			{
				stream << "https://facedatabase.oss-cn-shanghai.aliyuncs.com/data/";
				stream << temp;
				stream << ".jpg";
				std::cout << stream.str() << std::endl;
				HttpPostaliyuntwo(url, buff, buff2, len, len2, ak_id, ak_secret, stream.str(), filename1);
				//在这里加入https请求
				stream.str("");
			}
			//HttpPostaliyuntwo(url, buff, buff2, len, len2, ak_id, ak_secret, file, filename1);

		//hThread2 = CreateThread(NULL, 0, HttpPostaliyuntwo(url, buff, buff2, len, len2, ak_id, ak_secret), NULL, 0, &threadID); // 创建线程  
#endif 
		/*}
		break;*/
	}
	/*case 2:
		if (aos_http_io_initialize(NULL, 0) != AOSE_OK) {
			return -1;
		}*/
	/*	delete_object();
		break;
	}*/
	getchar();
	return 0;
}