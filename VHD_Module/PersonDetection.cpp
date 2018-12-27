#include "stdafx.h"
#include "PersonDetection.h"




PersonDetector::PersonDetector() {
	cout << "PersonDetector has been created " << endl;
}
int PersonDetector::VHD_Init() {
	Status loadGraphStatus = loadGraph(graphPath, &session);
	if (!loadGraphStatus.ok()) {
		LOG(ERROR) << "loadGraph(): ERROR" << loadGraphStatus;
		return -1;
	}
	LOG(INFO) << "loadGraph(): frozen graph loaded" << endl;	
	return 0;
}
int PersonDetector::VHD_Uninit() {
	this->session->Close();
	return 0;
}
int PersonDetector::VHD_Reset() {
	this->session->Close();
	return VHD_Init();
}
int PersonDetector::VHD_SetImageSize(int nWidth, int nHeight, int nImgType) {
	detImage.imgWidth = nWidth;
	detImage.imgHeight = nHeight;
	detImage.nImgType = nImgType;
	if (nImgType == 1) {
		this->tmpFile = "tmp_000001.png";
	}
	else if (nImgType == 2) {
		this->tmpFile = "tmp_000002.jpeg";
	}
	else {
		this->tmpFile = "tmp_000003.gif";
	}
	return 0;
}
int PersonDetector::VHD_InputImage(INPersonDetect* pInPersonDetect, char* pImgBuf, int nSize) {
	INPersonDetect* res = new INPersonDetect;
	
	std::vector<Tensor> resized_tensors;

	LOG(INFO) << "formatTensor() successful" << endl;
	Status read_tensor_status = ReadTensorFromImageFile(pImgBuf,nSize,&resized_tensors);
	LOG(INFO) << "ReadTensorFromImageFile() successful" << endl;
	if (!read_tensor_status.ok()) {
		LOG(ERROR) << read_tensor_status;
		return -1;
	}
	const Tensor& resized_tensor = resized_tensors[0];
	std::vector<Tensor> outputs;
	Status run_status = session->Run({ { input_layer, resized_tensor } }, output_layer, {}, &outputs);
	LOG(INFO) << "session run() successful" << endl;
	if (!run_status.ok()) {
		LOG(ERROR) << "Running model failed: " << run_status;
		return -1;
	}
	tensorflow::TTypes<float>::Flat scores = outputs[1].flat<float>();
	tensorflow::TTypes<float>::Flat classes = outputs[2].flat<float>();
	tensorflow::TTypes<float>::Flat num_detections = outputs[3].flat<float>();
	auto boxes = outputs[0].flat_outer_dims<float, 3>();
	//tensorflow::TTypes<int>::Flat image_flat = (&resized_tensors[1])->flat<int>();
	int nums = 0;
	if (pInPersonDetect != NULL)
	{
		pInPersonDetect->persongeos = NULL;
		if (num_detections(0) > 0)
		{
			pInPersonDetect->persongeos = new INPersonGeo[num_detections(0)];
			for (size_t i = 0; i < num_detections(0); ++i)
			{
				if (scores(i) > thresholdScore && classes(i) == 1)
				{
					float top = boxes(0, i, 0);
					float left = boxes(0, i, 1);
					float bottom = boxes(0, i, 2);
					float right = boxes(0, i, 3);
					float score = scores(i);
					
					pInPersonDetect->persongeos[nums] = { score,left,top,right,bottom };
					//LOG(INFO) << i << ",score:" << scores(i) << ",class:" << classes(i) << ",box:" << "," << left << "," << top << "," << right << "," << bottom;
					nums++;
				}
			}
			pInPersonDetect->numDetections = nums;
			pInPersonDetect->state = 0;
		}
	}	
	return 0;
}
int PersonDetector::VHD_SetAlgMode(int nType) {
	return 0;
}

	//创建session并加载模型
Status PersonDetector::loadGraph(const string &graph_file_name, std::unique_ptr<tensorflow::Session> *session) {
	tensorflow::GraphDef graph_def;
	Status load_graph_status = ReadBinaryProto(tensorflow::Env::Default(), graph_file_name, &graph_def);
	if (!load_graph_status.ok()) {
		return Status(tensorflow::error::NOT_FOUND,"Failed to load compute graph at" + graph_file_name + "'");
	}
	session->reset(tensorflow::NewSession(tensorflow::SessionOptions()));
	Status session_create_status = (*session)->Create(graph_def);
	if (!session_create_status.ok()) {
		return session_create_status;
	}
	return Status::OK();
}

//Status PersonDetector::ReadEntireFile(tensorflow::Env* env, const string& filename, Tensor* output) {
//	tensorflow::uint64 file_size = 0;
//	TF_RETURN_IF_ERROR(env->GetFileSize(filename, &file_size));
//
//	string contents;
//	contents.resize(file_size);
//
//	std::unique_ptr<tensorflow::RandomAccessFile> file;
//	TF_RETURN_IF_ERROR(env->NewRandomAccessFile(filename, &file));
//
//	tensorflow::StringPiece data;
//	TF_RETURN_IF_ERROR(file->Read(0, file_size, &data, &(contents)[0]));
//	if (data.size() != file_size) {
//		return tensorflow::errors::DataLoss("Truncated read of '", filename,
//			"' expected ", file_size, " got ",
//			data.size());
//	}
//	output->scalar<string>()() = data.ToString();
//	
//	return Status::OK();
//}

//将图片数据转换为Tensor
Status PersonDetector::ReadTensorFromImageFile(char* pImgBuf, int length, std::vector<Tensor>* out_tensors) {
	auto root = tensorflow::Scope::NewRootScope();
	using namespace ::tensorflow::ops;  // NOLINT(build/namespaces)
	//将文件名读入一个名为input的张量
	Tensor input(tensorflow::DT_STRING, tensorflow::TensorShape());
	tensorflow::StringPiece data;
	*(&data) = tensorflow::StringPiece(pImgBuf, length);
	input.scalar<string>()() = data.ToString();

	//使用占位符读取输入数据
	auto file_reader = Placeholder(root.WithOpName("input"), tensorflow::DataType::DT_STRING);
	std::vector<std::pair<string, tensorflow::Tensor>> inputs = {
		{ "input", input },
	};

	const int wanted_channels = 3;
	tensorflow::Output image_reader;
	if (this->detImage.nImgType == 1) {
		image_reader = DecodePng(root.WithOpName("png_reader"), file_reader,
			DecodePng::Channels(wanted_channels));
	}
	else if (this->detImage.nImgType == 2) {
		image_reader = DecodeJpeg(root.WithOpName("jpeg_reader"), file_reader,
			DecodeJpeg::Channels(wanted_channels));
	}
	else if (this->detImage.nImgType == 3) {
		image_reader = DecodeBmp(root.WithOpName("bmp_reader"), file_reader,
			DecodeBmp::Channels(wanted_channels));
	}
	else {
		image_reader = Squeeze(root.WithOpName("squeeze_first_dim"),
				DecodeGif(root.WithOpName("gif_reader"), file_reader));
	}
	auto uint8_caster = Cast(root.WithOpName("uint8_caster"), image_reader, tensorflow::DT_UINT8);
	auto dims_expander = ExpandDims(root.WithOpName("dim"), uint8_caster, 0);
	tensorflow::GraphDef graph;
	TF_RETURN_IF_ERROR(root.ToGraphDef(&graph));

	std::unique_ptr<tensorflow::Session> session(
		tensorflow::NewSession(tensorflow::SessionOptions()));
	TF_RETURN_IF_ERROR(session->Create(graph));
	TF_RETURN_IF_ERROR(session->Run({ inputs }, { "dim" }, {}, out_tensors));
	return Status::OK();
}
//int PersonDetector::formatTensor(unsigned char* pImgBuf,int length) {
//	try {
//		FILE* fp;
//		tensorflow::Env* env = tensorflow::Env::Default();
//		if (env->FileExists(tmpFile.c_str()).OK) {
//			env->DeleteFile(tmpFile.c_str());
//		}
//		fopen_s(&fp, tmpFile.c_str(), "wb");
//		//从buffer中写数据到fp指向的文件中
//		fwrite(pImgBuf, sizeof(unsigned char), length, fp);
//		fclose(fp);
//	}
//	catch (char* str) {
//		LOG(ERROR) << "formatTensor() error : " << str;
//		return -1;
//	}
//	return 0;
//	
//}
int  PersonDetector::drawOnImage(float left, float top, float right, float bottom, tensorflow::TTypes<int>::Flat* image)
{
	int image_width = detImage.imgWidth;
	int image_height = detImage.imgHeight;
	int b_left = (int)left * image_width;
	int b_top = (int)top * image_height;
	int b_right = (int)right * image_width;
	int b_bottom = (int)bottom * image_height;

	tensorflow::TTypes<int>::Flat image_ref = *image;

	top = std::max(0, std::min(image_height - 1, b_top));
	bottom = std::max(0, std::min(image_height - 1, b_bottom));

	left = std::max(0, std::min(image_width - 1, b_left));
	right = std::max(0, std::min(image_width - 1, b_right));

	for (int i = 0; i < 3; ++i) {
		uint8 val = i == 2 ? 255 : 0;
		for (int x = left; x <= right; ++x) {
			image_ref((top * image_width + x) * 3 + i) = val;
			image_ref((bottom * image_width + x) * 3 + i) = val;
		}
		for (int y = top; y <= bottom; ++y) {
			image_ref((y * image_width + left) * 3 + i) = val;
			image_ref((y * image_width + right) * 3 + i) = val;
		}
	}
	return 0;
}