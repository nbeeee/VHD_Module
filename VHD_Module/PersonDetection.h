#pragma once
#define _SCL_SECURE_NO_WARNINGS
#define COMPILER_MSVC



#include <fstream>
#include <utility>
#include <vector>
#include <list>
#include <iostream>
#include "tensorflow/c/c_api.h"
#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/graph/default_device.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/strings/str_util.h"
#include "tensorflow/core/lib/core/threadpool.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/util/command_line_flags.h"
#include "tensorflow/c/c_api.h"

#pragma comment(lib,"E:\\vscodespace\\tensorflow\\lib\\tensorflow.lib")

using tensorflow::Flag;
using tensorflow::Tensor;
using tensorflow::Status;
using tensorflow::string;
using tensorflow::uint8;
using namespace std;



//传入图片信息
struct DetImage {
	int imgWidth;
	int imgHeight;
	int nImgType;
	string imgType;
};
typedef  struct{
	float score,left, top, right, bottom;
}INPersonGeo;

typedef struct {
	int state = -1;
	int numDetections;
	INPersonGeo* persongeos;
}INPersonDetect;

class PersonDetector {
protected:
	DetImage detImage;
	std::unique_ptr<tensorflow::Session> session;
	double thresholdScore = 0.5;
	//string labelPath = "model/mscoco_label_map.pbtxt";
	string tmpFile = "tmp_000001";
	string graphPath = "model/frozen_inference_graph.pb";
	string input_layer = "image_tensor:0";
	vector<string> output_layer = { "detection_boxes:0", "detection_scores:0", "detection_classes:0", "num_detections:0" };
public:
	PersonDetector();
	//模型初始化
	int VHD_Init();
	//卸载模型
	int VHD_Uninit();
	//算法重置
	int VHD_Reset();
	//设置输入图片的宽，高，格式
	int VHD_SetImageSize(int nWidth, int nHeight, int nImgType);
	//输入图片，返回结果
	int VHD_InputImage(INPersonDetect* res ,char* pImgBuf, int nSize);
	//设置算法模式，单一算法或复用
	int VHD_SetAlgMode(int nType);

protected:
	Status loadGraph(const string &graph_file_name, std::unique_ptr<tensorflow::Session> *session);
	//Status ReadEntireFile(tensorflow::Env* env, const string& filename, Tensor* output);
	Status ReadTensorFromImageFile(char* pImgBuf, int length, std::vector<Tensor>* out_tensors);
	//int formatTensor(unsigned char* pImgBuf, int length);
	int drawOnImage(float left, float top, float right, float bottom, tensorflow::TTypes<int>::Flat* image);
};

