#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace cv;
#define levelMax 10

//Trạng thái di chuyển của con rắn 
enum TrangThai { UP, DOWN, LEFT, RIGHT };

struct ToaDo
{
	int y, x;
};

struct Dot
{
	ToaDo td;
	Dot*next;
};

struct ConRan
{
	Dot*head;
	int SoDot;
	TrangThai tt;
};

struct MoiAn
{
	ToaDo tdq;
};

// CÁC HÀM CHO CHẾ ĐỘ MỘT NGƯỜI CHƠI
Dot*ThemDot(int x1, int y1);
void ThemDau(ConRan&r, int x1, int y1);
void KhoiTaoRan(ConRan&r, MoiAn&ma);
void HienThi(ConRan&r, Mat&img, MoiAn ma);
int XuLi(ConRan&r, MoiAn &ma, int key, int &level);
void TruyenTrangThai(ConRan&r, Dot*p, Dot*k);
void DieuKhien_DiChuyen(ConRan&r, int &key);
void HuongDan(Mat img, int key);
void TaoManHinhPhu_Menu(Mat&img, FILE*f);
int XuliThuaGame_ThangGame(ConRan r, MoiAn ma, int key, int level, Mat&img);
void LuuDiemMax(FILE*&f, ConRan r);
//--------------------------------------------------------------------------------------------

//CÁC HÀM CHO CHẾ ĐỘ HAI NGƯỜI CHƠI
void KhoiTaoRan2(ConRan&r);
void HienThi2(ConRan&r1, ConRan&r2, MoiAn&ma, Mat&img);
void DieuKhien_DiChuyen2(ConRan&r1, ConRan&r2, int &key);
int XuLi2(ConRan&r1, ConRan&r2, MoiAn &ma, int key, int &level1, int &level2);
int XuliThuaGame_ThangGame2(ConRan r1, ConRan r2, MoiAn ma, int key, int level1, int level2, Mat&img);
void HuongDanHaiNguoiChoi(Mat img, int key);
//-------------------------------------------------------------------------------------------------------

//CHƯƠNG TRÌNH CHÍNH
int main()
{
	srand(time_t(NULL));
	Mat img(600, 1000, CV_32FC3);
	ConRan r, r1, r2;
	MoiAn ma;
	int level = 0, level1 = 0, level2 = 0;
	FILE*f = fopen("BangXepHang.txt", "a+");
	KhoiTaoRan2(r2);
	KhoiTaoRan(r1, ma);
	KhoiTaoRan(r, ma);
	TaoManHinhPhu_Menu(img, f);//Mành hình phụ:show điểm và chế độ chơi
	char LuaChon = waitKey(0);
	if (LuaChon == '2')//Chế độ hai người
	{
		img = Mat::zeros(img.rows, img.cols, CV_32FC3);
		int key = 'H';
		HuongDanHaiNguoiChoi(img, key);
		imshow("GAME CON RAN", img);
		while (1)
		{
			
			int key = waitKey(100);
			//Hướng dẫn chơi game
			HuongDanHaiNguoiChoi(img, key);
			//Hiện thị
			HienThi2(r1, r2, ma, img);
			//Điều khiển
			DieuKhien_DiChuyen2(r1, r2, key);
			//Xử lí cắn đuôi,thêm đốt,tăng level,chạm biên...
			XuLi2(r1, r2, ma, key, level1, level2);
			//Xử lí thua game thắng game
			if (XuliThuaGame_ThangGame2(r1, r2, ma, key, level1, level2, img) == 1)//Thua game,thắng game
			{
				break;
			}
			if (XuliThuaGame_ThangGame2(r1, r2, ma, key, level1, level2, img) == -1)// Nhấn nút Q để thoát game
			{

				break;
			}
			imshow("GAME CON RAN", img);
			img = Mat::zeros(img.rows, img.cols, CV_32FC3);
		}
	}
	else //Chế độ chơi một người
	{
		img = Mat::zeros(img.rows, img.cols, CV_32FC3);
		int key = 'H';
		HuongDan(img, key);
		imshow("GAME CON RAN", img);
		while (1)
		{
			int key = waitKey(101 - level * 10);
			//Hướng dẫn chơi game
			HuongDan(img, key);
			//Hiện thị
			HienThi(r, img, ma);
			//Điều khiển				
			DieuKhien_DiChuyen(r, key);
			//Xử lí cắn đuôi,thêm đốt,tăng level,chạm biên...
			XuLi(r, ma, key, level);
			//Xử lí thua game thắng game
			if (XuliThuaGame_ThangGame(r, ma, key, level, img) == 1)//Thua game,thắng game
			{
				break;
			}
			if (XuliThuaGame_ThangGame(r, ma, key, level, img) == -1)// Nhấn nút Q để thoát game
			{
				break;
			}
			imshow("GAME CON RAN", img);
			img = Mat::zeros(img.rows, img.cols, CV_32FC3);
		}
		LuuDiemMax(f, r);
		fclose(f);
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------------

//TOÀN BỘ QUÁ TRÌNH XỬ LÍ CON RẮN

//XỬ LÍ MỘT NGƯỜI CHƠI
Dot*ThemDot(int x1, int y1)
{
	Dot*p = new Dot;
	p->td.x = x1;
	p->td.y = y1;
	p->next = NULL;
	return p;
}
void ThemDau(ConRan&r, int x1, int y1)
{
	Dot*p = ThemDot(x1, y1);
	p->next = r.head;
	r.head = p;
	r.SoDot++;
}
//Khởi tạo con rắn ban đầu sẽ có một đốt
void KhoiTaoRan(ConRan&r, MoiAn&ma)
{
	r.head = NULL;
	Dot*p = ThemDot(40, 100);
	r.head = p;
	r.tt = RIGHT;
	r.SoDot = 1;
	ma.tdq.x = 300;
	ma.tdq.y = 300;
}
// Hiển thị con rắn,mồi ăn,biên và các phím chức năng
void HienThi(ConRan&r, Mat&img, MoiAn ma)
{
	char Diem[50];
	char Point1[100] = "Point:  ";
	_itoa_s(r.SoDot, Diem, 10);
	strcat_s(Point1, Diem);
	putText(img, Point1, Point(800, 60), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(0, 0, 255), 3);
	//Hiển thị level,mỗi khi ăn hai điểm sẽ tăng 1 level
	int level1 = r.SoDot / 2;
	char C_level[10];
	char M_level[19] = "Level:  ";
	_itoa_s(level1, C_level, 10);
	strcat_s(M_level, C_level);
	putText(img, M_level, Point(600, 60), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(0, 0, 255), 3);
	//Các phím chức năng,Q thoát,R đưa về trạng thái ban đầu,H bật trợ giúp
	putText(img, "Quit(Q)", Point(20, 60), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(0, 0, 255), 3);
	putText(img, "Restart(R)", Point(200, 60), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(0, 0, 255), 3);
	putText(img, "Help(H)", Point(420, 60), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(0, 0, 255), 3);
	//Vẽ biên
	rectangle(img, Point(20, 80), Point(980, 580), Scalar(0, 0, 255));
	// Hiện thị mồi ăn
	circle(img, Point(ma.tdq.x, ma.tdq.y), 1,
		Scalar(255, 0, 0), 20);
	//Hiển thị con rắn
	circle(img, Point(r.head->td.x, r.head->td.y), 1,       //Cái đầu hình tròn màu đỏ
		Scalar(0, 0, 255), 20);
	if (r.SoDot > 1)
	{
		for (Dot*p = r.head->next; p; p = p->next)
		{
			circle(img, Point(p->td.x, p->td.y), 1,       
				Scalar(0, 255, 0), 20);
		}
	}
}
// Truyền trạng thái cho các đốt sau cái đầu khi con rắn di chuyển
void TruyenTrangThai(ConRan&r, Dot*p, Dot*k)
{
	if (k == NULL)
	{
		return;
	}
	if (k->next == NULL)
	{
		return;
	}
	p = p->next;
	k = k->next;
	TruyenTrangThai(r, p, k);
	k->td = p->td;
}
// Điều khiển con rắn
void DieuKhien_DiChuyen(ConRan&r, int &key)
{
	Dot*p = r.head;
	Dot*k = r.head->next;
	TruyenTrangThai(r, p, k);
	if (r.head->next != NULL)
	{
		r.head->next->td = r.head->td;
	}
	if (key == 'A' || key == 'a')
		r.tt = LEFT;
	else if (key == 'D' || key == 'd')
	{
		r.tt = RIGHT;
	}
	else if (key == 'W' || key == 'w')
		r.tt = UP;
	else if (key == 'S' || key == 's')
		r.tt = DOWN;

	if (r.tt == UP)
		r.head->td.y = r.head->td.y - 20;
	else if (r.tt == DOWN)
		r.head->td.y = r.head->td.y + 20;
	else if (r.tt == LEFT)
		r.head->td.x = r.head->td.x - 20;
	else if (r.tt == RIGHT)
	{
		r.head->td.x = r.head->td.x + 20;
	}
}
//Xử lí cắn đuôi,thêm đốt,tăng level,chạm biên...
int XuLi(ConRan&r, MoiAn &ma, int key, int &level)
{
	//Ăn mồi thì sẽ thêm một đốt tại vị trí con mồi
	if (r.head->td.x == ma.tdq.x && r.head->td.y == ma.tdq.y)
	{
		ThemDau(r, ma.tdq.x, ma.tdq.y);
		ma.tdq.x = ((rand() % 47) + 1) * 20;
		ma.tdq.y = ((rand() % 24) + 4) * 20;
		level++;
	}
	//Xử lí rắn chạm biên
	if (r.head->td.x < 20 || r.head->td.y < 80 || r.head->td.x >= 1000 || r.head->td.y > 580)
	{
		return -1;
	}
	//Xử lí cắn đuôi:Lưu ý ba đốt trở lên mới có thể cán đuôi,3 đốt trở xuống có thể di chuyển ngược
	if (r.SoDot > 3)
	{
		for (Dot*p = r.head->next->next; p; p = p->next)
		{
			if (r.head->td.y == p->td.y&& r.head->td.x == p->td.x)
			{
				return -1;
			}
		}
	}
	// Xử lí các phím chức năng
	if (key == 'Q' || key == 'q')
	{
		return 0;
	}
	if (key == 'r' || key == 'R')
	{
		KhoiTaoRan(r, ma);
		return 2;
	}
	//Xử lí thắng khi đạt level 10
	level = r.SoDot / 2;
	if (level == 10)
		return 1;
	return 2;
}
// Hướng dẫn chơi game
void HuongDan(Mat img, int key)
{
	//Nếu nhập H sẽ hiện thị hướng dẫn
	if (key == 'h' || key == 'H')
	{
		rectangle(img, Point(20, 80), Point(980, 580), Scalar(0, 0, 255), 5);
		putText(img, "HUONG DAN", Point(400, 150), FONT_HERSHEY_SIMPLEX, 1,
			Scalar(0, 0, 255), 5);
		//Hướng dẫn di chuyển
		putText(img, "Phim di chuyen: A(sang trai),D(sang phai)",
			Point(30, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		putText(img, "W(len tren),S(Xuong Duoi)",
			Point(300, 240), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		//Hướng dẫn phím tắt
		putText(img, "Phim tat: Q(thoat),R(choi lai),H(huong dan)",
			Point(30, 280), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		//Hướng dẫn thắng thua
		putText(img, "Quy dinh thua: Dau can trung than hoac cham bien",
			Point(30, 320), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		//Quy định thắng 
		putText(img, "Quy dinh thang: Dat 20 diem,tuc la level 10",
			Point(30, 360), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		//Quy định về level
		putText(img, "Quy dinh level: moi 2 diem la 1 level",
			Point(30, 400), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		putText(img, "level cang cao toc do di chuyen cang nhanh",
			Point(30, 440), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		putText(img, "Nhap phim bat ki de tiep tuc choi!", Point(210, 560), FONT_HERSHEY_SIMPLEX, 1,
			Scalar(255, 0, 0), 3);
		imshow("GAME CON RAN", img);
		waitKey(10000);
	}
}
//Xử lí thắng thua game
int XuliThuaGame_ThangGame(ConRan r, MoiAn ma, int key, int level, Mat&img)
{
	if (XuLi(r, ma, key, level) == -1)//Thua game
	{
		putText(img, "GAME OVER", Point(200, 350), FONT_HERSHEY_SIMPLEX, 3,
			Scalar(0, 0, 255), 5);
		imshow("GAME CON RAN", img);
		waitKey(10000);
		return 1;
	}
	if (XuLi(r, ma, key, level) == 1)//Thắng game
	{
		putText(img, "YOU WIN", Point(200, 350), FONT_HERSHEY_SIMPLEX, 3,
			Scalar(255, 0, 0), 5);
		imshow("GAME CON RAN", img);
		waitKey(10000);
		return 1;
	}
	if (XuLi(r, ma, key, level) == 0)//Nhấn nút Q
	{
		return -1;
	}
	return 0;
}
//Mành hình phụ:show điểm và chế độ chơi
void TaoManHinhPhu_Menu(Mat&img, FILE*f)
{
	rectangle(img, Point(20, 80), Point(980, 580), Scalar(0, 0, 255), 5);
	putText(img, "GAME SNAKE", Point(200, 350), FONT_HERSHEY_SIMPLEX, 3,
		Scalar(0, 0, 255), 5);
	//hien diem cao nhat
	char Diem[3];
	fgets(Diem, 10, f);
	char Top[20] = "Diem cao nhat: ";
	strcat_s(Top, Diem);
	putText(img, Top, Point(350, 400), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(255, 0, 0), 3);

	putText(img, "Nhan phim 1 hai lan de choi mot nguoi", Point(210, 450), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(255, 0, 0), 3);
	putText(img, "Nhan phim 2 hai lan de choi hai nguoi", Point(210, 500), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(255, 0, 0), 3);
	imshow("GAME CON RAN", img);
	waitKey(10000);
}
//Lưu điểm cao nhất
void LuuDiemMax(FILE*&f, ConRan r)
{
	int diem1 = 0;
	int diem2 = 0;
	fprintf(f, " %d ", r.SoDot);
	rewind(f);
	fscanf(f, "%d", &diem1);
	fscanf(f, "%d", &diem2);
	f = fopen("BangXepHang.txt", "w");
	if (diem1 >= diem2)
	{
		fprintf(f, "%d", diem1);
	}
	else
	{
		fprintf(f, "%d", diem2);
	}

}
//---------------------------------------------------------------------------------------------------------

// CHẾ ĐỘ HAI NGƯỜI CHƠI
//Khởi tạo con rắn 2 ở tọa độ khác
void KhoiTaoRan2(ConRan&r)
{
	r.head = NULL;
	Dot*p = ThemDot(100, 200);
	r.head = p;
	r.tt = RIGHT;
	r.SoDot = 1;
}
void HienThi2(ConRan&r1, ConRan&r2, MoiAn&ma, Mat&img)
{
	//Hiển thị điểm rắn 1,2
	char Diem[50],Diem2[50],Point1[100] = "Point 1,2: ";
	_itoa_s(r2.SoDot, Diem2, 10);
	_itoa_s(r1.SoDot, Diem, 10);
	strcat_s(Point1, Diem);
	strcat_s(Point1, ",");
	strcat_s(Point1, Diem2);
	putText(img, Point1, Point(760, 60), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(0, 0, 255), 1);
	//Hiển thị level rắn 1,2
	int level1 = r1.SoDot / 2, level2 = r2.SoDot / 2;
	char C_level1[50],C_level2[50],M_level[50] = "Level 1,2:  ";
	_itoa_s(level1, C_level1, 10);
	_itoa_s(level2, C_level2, 10);
	strcat_s(M_level, C_level1);
	strcat_s(M_level, ",");
	strcat_s(M_level, C_level2);
	putText(img, M_level, Point(500, 60), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(0, 0, 255), 1);
	//Phím chức năng,R khơi tạo cả hai rắn
	putText(img, "Quit(Q)", Point(20, 60), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(0, 0, 255), 1);
	putText(img, "Restart(R)", Point(150, 60), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(0, 0, 255), 1);
	putText(img, "Help(H)", Point(350, 60), FONT_HERSHEY_SIMPLEX, 1,
		Scalar(0, 0, 255), 1);
	rectangle(img, Point(20, 80), Point(980, 580), Scalar(0, 0, 255));
	circle(img, Point(ma.tdq.x, ma.tdq.y), 1,
		Scalar(255, 0, 0), 20);
	//Hiển thị rắn 1
	circle(img, Point(r1.head->td.x, r1.head->td.y), 1,       //Cái đầu hình tròn màu đỏ
		Scalar(0, 0, 255), 20);
	if (r1.SoDot > 1)
	{
		for (Dot*p = r1.head->next; p; p = p->next)
		{
			circle(img, Point(p->td.x, p->td.y), 1,     
				Scalar(0, 255, 0), 20);
		}
	}
	//Hiển thị rắn 2
	circle(img, Point(r2.head->td.x, r2.head->td.y), 1,       //Cái đầu hình tròn màu xanh
		Scalar(255, 0, 0), 20);
	if (r2.SoDot > 1)
	{
		for (Dot*p = r2.head->next; p; p = p->next)
		{
			circle(img, Point(p->td.x, p->td.y), 1,     
				Scalar(0, 255, 0), 20);
		}
	}
}
void DieuKhien_DiChuyen2(ConRan&r1, ConRan&r2, int &key)
{
	//Truyền trạng thái cho rắn 1
	Dot*p = r1.head;
	Dot*k = r1.head->next;
	TruyenTrangThai(r1, p, k);
	if (r1.head->next != NULL)
	{
		r1.head->next->td = r1.head->td;
	}
	//Truyền trạng thái cho rắn 2
	Dot*p1 = r2.head;
	Dot*k1 = r2.head->next;
	TruyenTrangThai(r2, p1, k1);
	if (r2.head->next != NULL)
	{
		r2.head->next->td = r2.head->td;
	}
	if (key == 'A' || key == 'a')
		r1.tt = LEFT;
	else if (key == 'D' || key == 'd')
	{
		r1.tt = RIGHT;
	}
	else if (key == 'W' || key == 'w')
		r1.tt = UP;
	else if (key == 'S' || key == 's')
		r1.tt = DOWN;

	if (r1.tt == UP)
		r1.head->td.y = r1.head->td.y - 20;
	else if (r1.tt == DOWN)
		r1.head->td.y = r1.head->td.y + 20;
	else if (r1.tt == LEFT)
		r1.head->td.x = r1.head->td.x - 20;
	else if (r1.tt == RIGHT)
	{
		r1.head->td.x = r1.head->td.x + 20;
	}


	if (key == 'J' || key == 'j')
		r2.tt = LEFT;
	else if (key == 'L' || key == 'l')
	{
		r2.tt = RIGHT;
	}
	else if (key == 'I' || key == 'i')
		r2.tt = UP;
	else if (key == 'K' || key == 'k')
		r2.tt = DOWN;

	if (r2.tt == UP)
		r2.head->td.y = r2.head->td.y - 20;
	else if (r2.tt == DOWN)
		r2.head->td.y = r2.head->td.y + 20;
	else if (r2.tt == LEFT)
		r2.head->td.x = r2.head->td.x - 20;
	else if (r2.tt == RIGHT)
	{
		r2.head->td.x = r2.head->td.x + 20;
	}
}
int XuLi2(ConRan&r1, ConRan&r2, MoiAn &ma, int key, int &level1, int &level2)
{
	if (r1.head->td.x == ma.tdq.x && r1.head->td.y == ma.tdq.y)
	{
		ThemDau(r1, ma.tdq.x, ma.tdq.y);
		ma.tdq.x = ((rand() % 47) + 1) * 20;
		ma.tdq.y = ((rand() % 24) + 4) * 20;
		level1++;
	}
	if (r2.head->td.x == ma.tdq.x && r2.head->td.y == ma.tdq.y)
	{
		ThemDau(r2, ma.tdq.x, ma.tdq.y);
		ma.tdq.x = ((rand() % 47) + 1) * 20;
		ma.tdq.y = ((rand() % 24) + 4) * 20;
		level2++;
	}
	if (r1.head->td.x < 20 || r1.head->td.y < 80 || r1.head->td.x >= 1000 || r1.head->td.y > 580)
	{
		return -1;
	}
	if (r2.head->td.x < 20 || r2.head->td.y < 80 || r2.head->td.x >= 1000 || r2.head->td.y > 580)
	{
		return -12;
	}
	/*Xử lí hai rắn chạm nhau,nếu đầu rắn nào chạm trúng thân rắn khác thì rắn đó thua,với đk một trong hai
	rắn phải có 1 đốt trơ lên*/
	if (r1.SoDot > 1 || r2.SoDot > 1)
	{
		for (Dot*p = r2.head->next; p; p = p->next)
		{
			if (r1.head->td.y == p->td.y&& r1.head->td.x == p->td.x)
			{
				return -1;
			}
		}
		for (Dot*p = r1.head->next; p; p = p->next)
		{
			if (r2.head->td.y == p->td.y&& r2.head->td.x == p->td.x)
			{
				return -12;
			}
		}
	}
	if (r1.SoDot > 3)
	{
		for (Dot*p = r1.head->next->next; p; p = p->next)
		{
			if (r1.head->td.y == p->td.y&& r1.head->td.x == p->td.x)
			{
				return -1;
			}
		}
	}
	if (r2.SoDot > 3)
	{
		for (Dot*p = r2.head->next->next; p; p = p->next)
		{
			if (r2.head->td.y == p->td.y&& r2.head->td.x == p->td.x)
			{
				return -12;
			}
		}
	}
	if (key == 'Q' || key == 'q')
	{
		return 0;
	}
	if (key == 'r' || key == 'R')
	{
		KhoiTaoRan(r1, ma);
		KhoiTaoRan2(r2);
		return 2;
	}
	level1 = r1.SoDot / 2;
	if (level1 == 10)
		return 1;
	level2 = r2.SoDot / 2;
	if (level2 == 10)
		return 12;
	return 2;
}
int XuliThuaGame_ThangGame2(ConRan r1, ConRan r2, MoiAn ma, int key, int level1, int level2, Mat&img)
{
	if (XuLi2(r1, r2, ma, key, level1, level2) == -1)
	{
		putText(img, "GAME OVER R1", Point(200, 350), FONT_HERSHEY_SIMPLEX, 3,
			Scalar(0, 0, 255), 5);
		imshow("GAME CON RAN", img);
		waitKey(10000);
		return 1;
	}
	if (XuLi2(r1, r2, ma, key, level1, level2) == -12)
	{
		putText(img, "GAME OVER R2", Point(200, 350), FONT_HERSHEY_SIMPLEX, 3,
			Scalar(0, 0, 255), 5);
		imshow("GAME CON RAN", img);
		waitKey(10000);
		return 1;
	}
	if (XuLi2(r1, r2, ma, key, level1, level2) == 1)
	{
		putText(img, "YOU WIN R1", Point(200, 350), FONT_HERSHEY_SIMPLEX, 3,
			Scalar(255, 0, 0), 5);
		imshow("GAME CON RAN", img);
		waitKey(10000);
		return 1;
	}
	if (XuLi2(r1, r2, ma, key, level1, level2) == 12)
	{
		putText(img, "YOU WIN R2", Point(200, 350), FONT_HERSHEY_SIMPLEX, 3,
			Scalar(255, 0, 0), 5);
		imshow("GAME CON RAN", img);
		waitKey(10000);
		return 1;
	}
	if (XuLi2(r1, r2, ma, key, level1, level2) == 0)
	{
		return -1;
	}
	return 0;
}
void HuongDanHaiNguoiChoi(Mat img, int key)
{
	if (key == 'h' || key == 'H')
	{
		rectangle(img, Point(20, 80), Point(980, 580), Scalar(0, 0, 255), 5);
		putText(img, "HUONG DAN", Point(400, 150), FONT_HERSHEY_SIMPLEX, 1,
			Scalar(0, 0, 255), 5);
		putText(img, "Phim di chuyen ran 2: J(sang trai),L(sang phai)",
			Point(30, 240), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		putText(img, "I(len tren),K(Xuong Duoi)",
			Point(300, 280), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		putText(img, "Them mot so chuc nang cho che do 2 nguoi choi",
			Point(30, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		putText(img, "Thua: 2 ran tren mot dot,can trung than cua nhau",
			Point(30, 320), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		putText(img, "Thang:1 trong 2 con dat level 10",
			Point(30, 360), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		putText(img, "khong co toc do trong che do 2 nguoi",
			Point(30, 400), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0));
		putText(img, "Nhap phim bat ki de tiep tuc choi!", Point(210, 560), FONT_HERSHEY_SIMPLEX, 1,
			Scalar(255, 0, 0), 3);
		imshow("GAME CON RAN", img);
		waitKey(100000);
	}
}