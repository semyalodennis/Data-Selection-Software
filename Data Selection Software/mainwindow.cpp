/*This file is part of the Data Selection software for Hyperspectral images/data.

DEPARTMENT OF BIOSYSTEMS MACHINERY ENGINEERING
NON DESTRUCTIVE BIOSENSING LABORATORY
CHUNGNAM NATIONAL UNIVERSITY

SUPERVISOR: PROFESSOR BYOUNG-KWAN CHO
DEVELOPER: SEMYALO DENNIS*/

#include "mainwindow.h"
#include "./ui_mainwindow.h"

//Qt
#include <QFileDialog>
#include <QMessageBox>

//Opencv
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui_c.h>

//STL
#include <iostream>
#include <fstream>

//Gdal
#include <gdal_priv.h>

//Others
#include "gdalopencv.h"


using namespace cv;
using namespace std;

//Global Variables
Mat gray_jet;
Mat gray_jet_Cal;
Mat gray_img_Cal;

Mat image2_f;
Mat image2;

Mat Dark_ref;
Mat White_ref;

Mat Image_Calibrate_f;
Mat Image_Calibrate_8;

bool leftDown = false, leftup = false;
Point cor1, cor2;
Rect box;
//Mat Cropped_Image;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Window parameters
    setWindowTitle("Data Selection Software V1.0"); //Title of the application
    setWindowIcon(QIcon(":/rec/Icons/Data_Selection.png"));

    ui->stackedWidget->setCurrentIndex(1); //Default widget

    //Adding labels to the status bar
    ui->statusbar->addPermanentWidget(ui->label_file);
    ui->statusbar->addPermanentWidget(ui->label_height);
    ui->statusbar->addPermanentWidget(ui->label_width);
    ui->statusbar->addPermanentWidget(ui->label_type);
    ui->statusbar->addPermanentWidget(ui->label_bands);

    //Make the tooltips of action buttons visible
    ui->menuFile->setToolTipsVisible(true);
    ui->menuTask->setToolTipsVisible(true);

    //Set icons for data selection
    //Adding relative path (From resource file)
    QPixmap pic1(":/rec/Icons/rec.png");

    //Scaling with width and height of label
    int w1 = ui->LB_Rec->width();
    int h1 = ui->LB_Rec->height();
    ui->LB_Rec->setPixmap(pic1.scaled(w1, h1, Qt::KeepAspectRatio));



    //Set icons for home
    //Adding relative path (From resource file)
    QPixmap pic4(":/rec/Icons/Cmake..png");
    QPixmap pic5(":/rec/Icons/CNU_logo.png");
    QPixmap pic6(":/rec/Icons/cpp.png");
    QPixmap pic7(":/rec/Icons/HSILOGO3.png");
    QPixmap pic8(":/rec/Icons/pls.png");
    QPixmap pic9(":/rec/Icons/preprocess3.png");
    QPixmap pic10(":/rec/Icons/qt images.png");
    QPixmap pic11(":/rec/Icons/opencv_logo.png");

    //Add the icons to the label
    /*ui->LB_Cmake->setPixmap(pic4);
    ui->LB_CNU->setPixmap(pic5);
    ui->LB_cpp->setPixmap(pic6);
    ui->LB_HSI->setPixmap(pic7);
    ui->LB_PLS->setPixmap(pic8);
    ui->LB_preprocess->setPixmap(pic9);
    ui->LB_Qt->setPixmap(pic10);
    ui->LB_Opencv->setPixmap(pic11);*/

    //Scaling with width and height of label
    int w4 = ui->LB_Cmake->width();
    int h4 = ui->LB_Cmake->height();
    ui->LB_Cmake->setPixmap(pic4.scaled(w4, h4, Qt::KeepAspectRatio));

    int w5 = ui->LB_CNU->width();
    int h5 = ui->LB_CNU->height();
    ui->LB_CNU->setPixmap(pic5.scaled(w5, h5, Qt::KeepAspectRatio));

    int w6 = ui->LB_cpp->width();
    int h6 = ui->LB_cpp->height();
    ui->LB_cpp->setPixmap(pic6.scaled(w6, h6, Qt::KeepAspectRatio));

    int w7 = ui->LB_HSI->width();
    int h7 = ui->LB_HSI->height();
    ui->LB_HSI->setPixmap(pic7.scaled(w7, h7, Qt::KeepAspectRatio));

    int w8 = ui->LB_PLS->width();
    int h8 = ui->LB_PLS->height();
    ui->LB_PLS->setPixmap(pic8.scaled(w8, h8, Qt::KeepAspectRatio));

    int w9 = ui->LB_preprocess->width();
    int h9 = ui->LB_preprocess->height();
    ui->LB_preprocess->setPixmap(pic9.scaled(w9, h9, Qt::KeepAspectRatio));

    int w10 = ui->LB_Qt->width();
    int h10 = ui->LB_Qt->height();
    ui->LB_Qt->setPixmap(pic10.scaled(w10, h10, Qt::KeepAspectRatio));

    int w11 = ui->LB_Opencv->width();
    int h11 = ui->LB_Opencv->height();
    ui->LB_Opencv->setPixmap(pic11.scaled(w11, h11, Qt::KeepAspectRatio));



}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionData_Selection_triggered()
{
    //Empty the qlabels before opening page
    connect(ui->actionData_Selection, SIGNAL(triggered()), ui->LB_falseColor, SLOT(clear()));

    //Empty the qlabels in the status bar before opening page
    connect(ui->actionData_Selection, SIGNAL(triggered()), ui->label_bands, SLOT(clear()));
    connect(ui->actionData_Selection, SIGNAL(triggered()), ui->label_file, SLOT(clear()));
    connect(ui->actionData_Selection, SIGNAL(triggered()), ui->label_height, SLOT(clear()));
    connect(ui->actionData_Selection, SIGNAL(triggered()), ui->label_type, SLOT(clear()));
    connect(ui->actionData_Selection, SIGNAL(triggered()), ui->label_width, SLOT(clear()));

    //Setting the initial state of the radio buttons
    ui->RB_Rec->setChecked(true); //checked
    ui->RB_Ave_Spectra->setChecked(false); //Not checked
    ui->RB_All_Spectra->setChecked(true); //checked

    //Setting the initial state of the Check boxes (not checked)
    ui->CB_Dark->setChecked(false); //Not checked
    ui->CB_White->setChecked(false); //Not checked

    //Open the data selection widget
    ui->stackedWidget->setCurrentIndex(2);

    //First clear items in combo box and then later add items when HSI is loaded
    ui->CB_NBands->clear();
}

void MainWindow::on_PB_Load_Cube_clicked()
{
    //Create filter for hyperspectral image files (.bil)
    //QString filter = "HSI Data Cube(*.bil)";
    QString filter = "All Files(*.*)";

    //Create string for dynamic path
    QString file_name2 = QFileDialog::getOpenFileName(this, "Load Hyperspectral Image", qApp->applicationDirPath(), filter);

    //Reading .bil files (Datacube)
    String filename = file_name2.toStdString();
    KGDAL2CV Read_datacube;
    Mat image = Read_datacube.ImgReadByGDAL(filename, false);

    //Display Details of the datacube in status bar
    ui->label_file->setText("File: " + file_name2);
    ui->label_height->setText("                      Height: " + QString::number(image.size[0]));
    ui->label_width->setText("                       Width: " + QString::number(image.size[1]));
    ui->label_bands->setText("                       Number of bands: " + QString::number(image.channels()));
    if (image.depth() == 0) { ui->label_type->setText("Data type: 8-bit unsigned"); }
    else if (image.depth() == 1) { ui->label_type->setText("Data type: 8-bit signed"); }
    else if (image.depth() == 2) { ui->label_type->setText("Data type: 16-bit unsigned"); }
    else if (image.depth() == 3) { ui->label_type->setText("Data type: 16-bit signed"); }
    else if (image.depth() == 4) { ui->label_type->setText("Data type: 32-bit signed"); }
    else if (image.depth() == 5) { ui->label_type->setText("Data type: 32-bit float"); }
    else if (image.depth() == 6) { ui->label_type->setText("Data type: 64-bit float"); }
    else if (image.depth() == 7) { ui->label_type->setText("Data type: User Defined "); }

    //Convert unsigned 16bit image (0~65535) to 32 bit floating points (float) - values between 0 and 1
    //Mat image2_f; - Global Variable
    image.convertTo(image2_f, CV_32F, 1.0 / 65535.0);

    //Convert 32 bit image (0 - 1) to 8bit image (0 - 255)
    //Mat image2; - Global Variable
    image2_f.convertTo(image2, CV_8U, 255);

    //False Color Image (3 bands outside Visible region) - false color because all bands are outside visible region

    //Split Channels
    //vector<Mat> channels(image2.channels());
    //split(image2, channels);

    //Display bands
    /*for (int b = 0; b < image2.channels(); b++)
    {
        //imshow("Bands " + QString::number(b).toStdString(), channels[b]);
        imshow("Bands " , channels[b]);
        waitKey(500);
    }*/

    //Create vector to store the bands to be combined
    /*vector<Mat> array_to_merge;

    //Put the chosen bands into the vector
    array_to_merge.push_back(channels[5]);
    array_to_merge.push_back(channels[40]);
    array_to_merge.push_back(channels[80]);

    //Create a matrix object for the false color image
    Mat False_color;

    //Combine the three chosen bands
    cv::merge(array_to_merge, False_color);

    //Convert opencv matrix object to Qt pixmap
    QPixmap pix_false = ASM::cvMatToQPixmap(False_color);

    //Scaling with width and height of label
    int w = ui->LB_falseColor->width();
    int h = ui->LB_falseColor->height();

    //Display false color image in label
    ui->LB_falseColor->setPixmap(pix_false.scaled(w, h, Qt::KeepAspectRatio));*/

    //Grayscale image using one selected band
    /*Mat gray_img =  channels[40];

    // Apply the colormap:
    //Mat gray_jet; - Global variable
    applyColorMap(gray_img, gray_jet, COLORMAP_JET);

    //Convert opencv matrix object to Qt pixmap
    QPixmap pix_colormap = ASM::cvMatToQPixmap(gray_jet);

    //Scaling with width and height of label
    int w = ui->LB_falseColor->width();
    int h = ui->LB_falseColor->height();

    //Display gray colormap (jet) image in label
    ui->LB_falseColor->setPixmap(pix_colormap.scaled(w, h, Qt::KeepAspectRatio));*/

    //Add the number of bands to the qcombo box
    for (int i = 0; i < image.channels(); i++)
    {
        ui->CB_NBands->addItem(QString::number(i+1));

    }

    //Set the current index for the combo box
    ui->CB_NBands->setCurrentIndex(39);

    //Status
    QMessageBox::information(this, "Status", "Done");

}


//Checking/Unchecking the dark reference check box
void MainWindow::on_CB_Dark_stateChanged(int arg1)
{
    //int arg1- Qt::Unchecked (0), Qt::PartiallyChecked(1), Qt::Checked (2)
    if (arg1 == 0)
    {
        QMessageBox::information(this, "Attention", "Tick the box to input the dark reference image");

    }
    else if (arg1 == 2)
    {
        QString filter_d = "All Files(*.*)";

        //Create string for dynamic path
        QString filename_d = QFileDialog::getOpenFileName(this, "Load the Dark reference Image", qApp->applicationDirPath(), filter_d);

        //Reading .bil files (Datacube)
        KGDAL2CV Read_dark;
        Mat image_d = Read_dark.ImgReadByGDAL(filename_d.toStdString(), false);

        //Convert unsigned 16bit dark image (0~65535) to 32 bit floating points (float) - values between 0 and 1
        Mat image_d2_f;
        image_d.convertTo(image_d2_f, CV_32F, 1.0 / 65535.0);

        //Convert 16bit image to 8bit image
        //Mat image_d2;
        //image_d.convertTo(image_d2, CV_8U, 1 / 256.0);

        //QMessageBox::information(this, "Number of channels", QString::number(image_d2.channels()));
        //QMessageBox::information(this, "Height of dark image", QString::number(image_d2.size[0]));//30
        //QMessageBox::information(this, "Width of dark image", QString::number(image_d2.size[1]));//160
        //QMessageBox::information(this, "Height of dark image", QString::number(image_d2.rows));//30
        //QMessageBox::information(this, "Width of dark image", QString::number(image_d2.cols));//160
        //Note: Height of sample HSI data cube - 200
        //Note: width of sample HSI data cube - 160

        //Reconstructing the dark image to fit size of sample data cube
        //Split Channels of dark image
        vector<Mat> channels(image_d2_f.channels());
        split(image_d2_f, channels);

        //Vector to store Average (mean) of each column for each band
        vector<Mat> col_mean(image_d2_f.channels());

        //Repeat obtained vector along vertical axis
        //Matrix vector to store obtained matrix
        vector<Mat> Repeat_colMean(image_d2_f.channels());
        int height = image2_f.rows;

        for (int b = 0; b < image_d2_f.channels(); b++)
        {
            //Average (mean) of each column for each band
            reduce(channels[b], col_mean[b], 0, REDUCE_AVG);

            //Repeat obtained vector along vertical axis
            repeat(col_mean[b], height, 1, Repeat_colMean[b]);

        }

        //Combine the bands
        //Mat Dark_ref; - global variable
        merge(Repeat_colMean, Dark_ref);

        //QMessageBox::information(this, "Height of New dark image", QString::number(Dark_ref.rows));
        //QMessageBox::information(this, "Width of New dark image", QString::number(Dark_ref.cols));

        //Status
        QMessageBox::information(this, "Status", "Done");



    }
}

//Checking/Unchecking the white reference check box - alternative method to statechanged
void MainWindow::on_CB_White_clicked(bool checked)
{
    if (checked)
    {
        QString filter_w = "All Files(*.*)";

        //Create string for dynamic path
        QString filename_w = QFileDialog::getOpenFileName(this, "Load the white reference Image", qApp->applicationDirPath(), filter_w);

        //Reading .bil files (Datacube)
        KGDAL2CV Read_white;
        Mat image_w = Read_white.ImgReadByGDAL(filename_w.toStdString(), false);

        //Convert 16bit image to 8bit image
        //Mat image_w2;
        //image_w.convertTo(image_w2, CV_8U, 1 / 256.0);

        //Convert unsigned 16bit dark image (0~65535) to 32 bit floating points (float) - values between 0 and 1
        Mat image_w2_f;
        image_w.convertTo(image_w2_f, CV_32F, 1.0 / 65535.0);

        //QMessageBox::information(this, "Number of channels", QString::number(image_w2.channels()));
        //QMessageBox::information(this, "Height of white image", QString::number(image_w2.size[0]));//30
        //QMessageBox::information(this, "Width of white image", QString::number(image_w2.size[1]));//160
        //Note: Height of sample HSI data cube - 200
        //Note: width of sample HSI data cube - 160

        //Reconstructing the white image to fit size of sample data cube
        //Split Channels of white image
        vector<Mat> channels(image_w2_f.channels());
        split(image_w2_f, channels);

        //Vector to store Average (mean) of each column for each band
        vector<Mat> col_mean(image_w2_f.channels());

        //Repeat obtained vector along vertical axis
        //Matrix vector to store obtained matrix
        vector<Mat> Repeat_colMean(image_w2_f.channels());
        int height = image2.rows;

        for (int b = 0; b < image_w2_f.channels(); b++)
        {
            //Average (mean) of each column for each band
            reduce(channels[b], col_mean[b], 0, REDUCE_AVG);

            //Repeat obtained vector along vertical axis
            repeat(col_mean[b], height, 1, Repeat_colMean[b]);

        }

        //Combine the bands
        //Mat White_ref; - global variable
        merge(Repeat_colMean, White_ref);

        //QMessageBox::information(this, "Height of New dark image", QString::number(White_ref.rows));
        //QMessageBox::information(this, "Width of New dark image", QString::number(White_ref.cols));

        //Status
        QMessageBox::information(this, "Status", "Done");



    }else if (!checked)
    {
        QMessageBox::information(this, "Attention", "Tick the box to input the white reference image");
    }
}

//Performing image calibration
void MainWindow::on_PB_ImgCalib_clicked()
{
    if (ui->CB_Dark->isChecked() && ui->CB_White->isChecked() )
    {
        //Split Channels for sample, dark and white reference images
        vector<Mat> channels(image2_f.channels());
        vector<Mat> channels3(Dark_ref.channels());
        vector<Mat> channels4(White_ref.channels());
        split(image2_f, channels);
        split(Dark_ref, channels3);
        split(White_ref, channels4);

        //Create Vectors for storing outputs
        vector<Mat> image_subtract1(image2_f.channels());
        vector<Mat> image_subtract2(image2_f.channels());
        vector<Mat> image_Calibrated(image2_f.channels());

        //Perform image calibration on each band. (sample - dark)/(white - dark)
        for (int b = 0; b < image2_f.channels(); b++) {
            //Subtract dark image from sample image
            subtract(channels[b], channels3[b], image_subtract1[b]);

            //Subtract dark image from white image
            subtract(channels4[b], channels3[b], image_subtract2[b]);

            //Division of resultant outputs from subtraction
            divide(image_subtract1[b], image_subtract2[b], image_Calibrated[b]);

        }

        //Combine the bands of calibrated image
        //Mat Image_Calibrate_f; - global variable
        merge(image_Calibrated, Image_Calibrate_f);

        //Convert 32bit float image to 8bit image
        //Mat Image_Calibrate_8; - global variable
        Image_Calibrate_f.convertTo(Image_Calibrate_8, CV_8U, 255);


        //QMessageBox::information(this, "Height of calibrated image", QString::number(Image_Calibrate.rows));
        //QMessageBox::information(this, "Width of calibrated image", QString::number(Image_Calibrate.cols));
        //QMessageBox::information(this, "Number of bands of calibrated image", QString::number(Image_Calibrate.channels()));

        //Display the bands of calibrated image
        //Split Channels
        /*vector<Mat> channels_calib(image2.channels());
        split(image2, channels_calib);

        //Display bands
        for (int b = 0; b < image2.channels(); b++)
        {
            //imshow("Bands " + QString::number(b).toStdString(), channels_calib[b]);
            imshow("Bands " , channels_calib[b]);
            waitKey(500);
        }*/

        //Status
         QMessageBox::information(this, "Status", "Done");


        //QMessageBox::information(this, "Attention", "Image Calibration");

    }else
    {
        QMessageBox::information(this, "Attention", "Load the dark and white reference images");
    }
}


//Saving mat object to txt file
//Saving all spectra to a text file
int WriteData(string fileName, cv::Mat& matData)
{

    int retVal = 0;

    //  Open the file
    ofstream outFile(fileName.c_str(), ios_base::out); // Write as new or overridden
    if (!outFile.is_open())
    {
        cout << " Failed to open file " << endl;
        retVal = -1;
        return (retVal);
    }

    //  Check that the matrix is empty
    if (matData.empty())
    {
        cout << " The matrix of the empty " << endl;
        retVal = 1;
        return (retVal);
    }

    //  Write data  (one band)
    /*for (int r = 0; r < matData.rows; r++)
    {
        for (int c = 0; c < matData.cols; c++)
        {
            //uchar data = matData.at<uchar>(r, c);
            Scalar data = matData.at<uchar>(r, c);// Read the data, at<type> - type  Is the specific data format of a matrix element
            outFile << data.val[0] << endl; // For each column of data  tab  separated
            //outFile << matData.at<uchar>(r, c) << "\t";
            //outFile << matData.at<double>(r, c) << "\t";
        }
        //outFile << "\t" ; // A newline
    }*/

    //Many bands
    //My way - band by band
    //Split Channels
    vector<Mat> channels(matData.channels());
    split(matData, channels);


    /*for (int b = 0; b < matData.channels(); b++) {
        for (int i = 0; i < matData.rows; i++)
        {
            for (int j = 0; j < matData.cols; j++)
            {
                Scalar data = channels[b].at<uchar>(i, j);// Read the data, at<type> - type  Is the specific data format of a matrix element
                //outFile << data.val[0] << endl; // All elements of each band in 1 column
                outFile << data.val[0] << "\t";

            }
            //outFile << endl;
        }

        outFile << endl;
        //outFile.seekp(0, ios::beg);

        //outFile << "\t"; // tab for other bands (columns separated by tab)

    }*/

    //Saving the pixel values so that the each column represents all the pixels in each band
    for (int i = 0; i < matData.rows; i++ ) {
        for (int j = 0; j < matData.cols; j++)
        {
            for (int b = 0; b < matData.channels(); b++ )
            {
                //Scalar data = channels[b].at<uchar>(i, j);// Read the data, at<type> - type  Is the specific data format of a matrix element
                Scalar data = channels[b].at<float>(i, j);
                //outFile << data.val[0] << endl; // All elements of each band in 1 column
                outFile << data.val[0] << "\t";

            }
            outFile << endl;
        }

        //outFile << endl;
        //outFile.seekp(0, ios::beg);

        //outFile << "\t"; // tab for other bands (columns separated by tab)

    }

    outFile.close();

    return (retVal);
}

//Saving average spectra (C++ vector) to a text file
int WriteVector(string fileName, vector<float> vecdata )
{

    int retVal2 = 0;

    //  Open the file
    ofstream outFile2(fileName.c_str(), ios_base::out); // Write as new or overridden
    if (!outFile2.is_open())
    {
        cout << " Failed to open file " << endl;
        retVal2 = -1;
        return (retVal2);
    }

    //  Check whether the vector is empty
    if (vecdata.empty())
    {
        cout << " The vector is empty " << endl;
        retVal2 = 1;
        return (retVal2);
    }

    int N_Vec_Elements = vecdata.size();

    for(int i = 0; i < N_Vec_Elements; i++)
    {
        outFile2 << vecdata[i] << "\t";
    }

    outFile2.close();

    return (retVal2);
}



/*
 * CALL BACK FUNCTIONS FOR ROI USING MOUSE
 *
 * CREATED CALL BACK FUNCTIONS FOR NON CALIBRATED IMAGE, CALIBRATED IMAGE, SAVING ALL SPECTRA, SAVING AVERAGE SPECTRA
 *
*/

//NON CALIBRATED IMAGE
//Callback function for selecting groups of data - save all spectra (Non Calibrated Image)
void mouse_call_NonCal_all(int event, int x, int y, int, void*)
{

    if (event == EVENT_LBUTTONDOWN)
    {
        leftDown = true;
        cor1.x = x;
        cor1.y = y;
        cout << "Corner 1: " << cor1 << endl;

    }
    if (event == EVENT_LBUTTONUP)
    {
        leftup = true;
        cor2.x = x;
        cor2.y = y;
        cout << "Corner 2: " << cor2 << endl;
    }

    if (leftDown == true && leftup == false) //when the left button is down
    {
        Point pt;
        pt.x = x;
        pt.y = y;

        Mat temp_img = gray_jet.clone();
        rectangle(temp_img, cor1, pt, Scalar(0, 0, 255), 1); //drawing a rectangle continuously
        imshow("Select and Save Groups", temp_img);

    }
    if (leftDown == true && leftup == true) //when the selection is done
    {

        box.width = abs(cor1.x - cor2.x);
        box.height = abs(cor1.y - cor2.y);
        box.x = min(cor1.x, cor2.x);
        box.y = min(cor1.y, cor2.y);

        leftDown = false;
        leftup = false;

        // Crop image for all  the bands
        Mat crop (image2_f, box);

        Mat Cropped_Image; //A global variable when selectROIs function is used

        Cropped_Image = crop;

        //Save selected image to .txt file (each band in a row)
        //Use filter for file extensions
       QString filter = "Text File(*.txt)";
       QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Selected data in a text file (each band in a row)", qApp->applicationDirPath(), filter);

        WriteData(fileName.toStdString(), Cropped_Image);

    }
}

//Callback function for selecting groups of data - save average spectra (Non Calibrated Image)
void mouse_call_NonCal_ave(int event, int x, int y, int, void*)
{

    if (event == EVENT_LBUTTONDOWN)
    {
        leftDown = true;
        cor1.x = x;
        cor1.y = y;
        cout << "Corner 1: " << cor1 << endl;

    }
    if (event == EVENT_LBUTTONUP)
    {
        leftup = true;
        cor2.x = x;
        cor2.y = y;
        cout << "Corner 2: " << cor2 << endl;
    }

    if (leftDown == true && leftup == false) //when the left button is down
    {
        Point pt;
        pt.x = x;
        pt.y = y;
        Mat temp_img = gray_jet.clone();
        rectangle(temp_img, cor1, pt, Scalar(0, 0, 255), 1); //drawing a rectangle continuously
        imshow("Select and Save Groups", temp_img);

    }
    if (leftDown == true && leftup == true) //when the selection is done
    {

        box.width = abs(cor1.x - cor2.x);
        box.height = abs(cor1.y - cor2.y);
        box.x = min(cor1.x, cor2.x);
        box.y = min(cor1.y, cor2.y);

        leftDown = false;
        leftup = false;

        // Crop image for all  the bands
        Mat crop (image2_f, box);

        Mat Cropped_Image; //A global variable when selectROIs function is used

        Cropped_Image = crop;

        //Split Channels for cropped image
        vector<Mat> channels3(Cropped_Image.channels());
        split(Cropped_Image, channels3);


        //Create Vector for storing average/mean value for the bands
        vector<float> Mean_spectra;

        //Compute the mean of each band (1 value returned per band)
        for (int b = 0; b < Cropped_Image.channels(); b++)
        {
            Scalar tempVal = mean(channels3[b]);

            //cout << "Intensity: " << tempVal.val[0] << endl;

            Mean_spectra.push_back(tempVal.val[0]);

        }

        //QMessageBox::information(nullptr, "The size of mean spectra vector", QString::number(Mean_spectra.size()));

        //Save selected image to .txt file (each band in a row)
        //Use filter for file extensions
       QString filter = "Text File(*.txt)";
       QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Selected data in a text file (each band in a row)", qApp->applicationDirPath(), filter);

        WriteVector(fileName.toStdString(), Mean_spectra );

    }
}


//CALIBRATED IMAGE
//Callback function for selecting groups of data - save all spectra (Calibrated Image)
void mouse_call_Cal_all(int event, int x, int y, int, void*)
{

    if (event == EVENT_LBUTTONDOWN)
    {
        leftDown = true;
        cor1.x = x;
        cor1.y = y;
        cout << "Corner 1: " << cor1 << endl;

    }
    if (event == EVENT_LBUTTONUP)
    {
        //if (abs(x - cor1.x) > 1 && abs(y - cor1.y) > 1) //checking whether the region is too small(changed it from 20 to 1)
        //{
            //leftup = true;
            //cor2.x = x;
            //cor2.y = y;
            //cout << "Corner 2: " << cor2 << endl;
        //}
        //else
        //{
            //cout << "Select a region more than 1 pixel" << endl;
        //}

        leftup = true;
        cor2.x = x;
        cor2.y = y;
        cout << "Corner 2: " << cor2 << endl;
    }

    if (leftDown == true && leftup == false) //when the left button is down
    {
        Point pt;
        pt.x = x;
        pt.y = y;

        //Mat temp_img = gray_jet.clone();
        Mat temp_img = gray_jet_Cal.clone();
        //Mat temp_img = gray_img_Cal.clone();
        rectangle(temp_img, cor1, pt, Scalar(0, 0, 255), 1); //drawing a rectangle continuously
        imshow("Select and Save Groups", temp_img);

    }
    if (leftDown == true && leftup == true) //when the selection is done
    {

        box.width = abs(cor1.x - cor2.x);
        box.height = abs(cor1.y - cor2.y);
        box.x = min(cor1.x, cor2.x);
        box.y = min(cor1.y, cor2.y);

        leftDown = false;
        leftup = false;

        // Crop image for all  the bands
        Mat crop (Image_Calibrate_f, box);

        Mat Cropped_Image; //A global variable when selectROIs function is used

        Cropped_Image = crop;

        //Save selected image to .txt file (each band in a row)
        //Use filter for file extensions
       QString filter = "Text File(*.txt)";
       QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Selected data in a text file (each band in a row)", qApp->applicationDirPath(), filter);

        WriteData(fileName.toStdString(), Cropped_Image);

    }
}

//Callback function for selecting groups of data - save average spectra (Calibrated Image)
void mouse_call_Cal_ave(int event, int x, int y, int, void*)
{

    if (event == EVENT_LBUTTONDOWN)
    {
        leftDown = true;
        cor1.x = x;
        cor1.y = y;
        cout << "Corner 1: " << cor1 << endl;

    }
    if (event == EVENT_LBUTTONUP)
    {
        leftup = true;
        cor2.x = x;
        cor2.y = y;
        cout << "Corner 2: " << cor2 << endl;
    }

    if (leftDown == true && leftup == false) //when the left button is down
    {
        Point pt;
        pt.x = x;
        pt.y = y;
        //Mat temp_img = gray_jet.clone();
        Mat temp_img = gray_jet_Cal.clone();
        //Mat temp_img = gray_img_Cal.clone();
        rectangle(temp_img, cor1, pt, Scalar(0, 0, 255), 1); //drawing a rectangle continuously
        imshow("Select and Save Groups", temp_img);

    }
    if (leftDown == true && leftup == true) //when the selection is done
    {

        box.width = abs(cor1.x - cor2.x);
        box.height = abs(cor1.y - cor2.y);
        box.x = min(cor1.x, cor2.x);
        box.y = min(cor1.y, cor2.y);

        leftDown = false;
        leftup = false;

        // Crop image for all  the bands
        Mat crop (Image_Calibrate_f, box);

        Mat Cropped_Image; //A global variable when selectROIs function is used

        Cropped_Image = crop;

        //Split Channels for cropped image
        vector<Mat> channels3(Cropped_Image.channels());
        split(Cropped_Image, channels3);


        //Create Vector for storing average/mean value for the bands
        vector<float> Mean_spectra;

        //Compute the mean of each band (1 value returned per band)
        for (int b = 0; b < Cropped_Image.channels(); b++)
        {
            Scalar tempVal = mean(channels3[b]);

            //cout << "Intensity: " << tempVal.val[0] << endl;

            Mean_spectra.push_back(tempVal.val[0]);

        }

        //QMessageBox::information(nullptr, "The size of mean spectra vector", QString::number(Mean_spectra.size()));

        //Save selected image to .txt file (each band in a row)
        //Use filter for file extensions
       QString filter = "Text File(*.txt)";
       QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Selected data in a text file (each band in a row)", qApp->applicationDirPath(), filter);

        WriteVector(fileName.toStdString(), Mean_spectra );

    }
}

void MainWindow::on_PB_Select_Groups_clicked()
{
    //Select multiple region of interests (selectROIs opencv function)
    // Specify a vector of rectangles (ROI)
    /*vector<Rect> rects;
    bool fromCenter = false;
    // The selected rectangles are in
    selectROIs("Select and Save Groups", gray_jet, rects, fromCenter);

    // Crop image
    Mat Cropped_Image;
    for (int i = 0; i < rects.size(); i++)
    {
        Cropped_Image = image2(rects[i]);

        //Save selected image to .txt file (each band in a row)
        //Use filters for file extensions
       QString filter = "Text File(*.txt)";
       QString fileName = QFileDialog::getSaveFileName(this, "Save Selected data in a text file (each band in a row)", qApp->applicationDirPath(), filter);

        WriteData(fileName.toStdString(), Cropped_Image);

    }*/

    //Formulated function
    //namedWindow("Select and Save Groups", WINDOW_FREERATIO);
    //imshow("Select and Save Groups", gray_jet);

    /*if(ui->RB_Rec->isChecked())
    {
        if(ui->RB_All_Spectra->isChecked())
        {
            //setting the mouse callback for selecting the region with mouse
            setMouseCallback("Select and Save Groups", mouse_call_all);

        }else if(ui->RB_Ave_Spectra->isChecked())
        {
            //setting the mouse callback for selecting the region with mouse
            setMouseCallback("Select and Save Groups", mouse_call_ave);

        }
    }
    else if(ui->RB_Cir->isChecked())
    {
        QMessageBox::information(this, "Attention", "Use rectangular selection tool");
    }
    else if(ui->RB_FreeForm->isChecked())
    {
        QMessageBox::information(this, "Attention", "Use rectangular selection tool");
    }*/




    //Calibrated Image
    //Split Channels
    //vector<Mat> channels2(Image_Calibrate_8.channels());
    //split(Image_Calibrate_8, channels2);

    //Display bands
    /*for (int b = 0; b < Image_Calibrate.channels(); b++)
    {
        //imshow("Bands " + QString::number(b).toStdString(), channels[b]);
        imshow("Bands " , channels[b]);
        waitKey(500);
    }*/

    //Create vector to store the bands to be combined
    /*vector<Mat> array_to_merge;

    //Put the chosen bands into the vector
    array_to_merge.push_back(channels[5]);
    array_to_merge.push_back(channels[40]);
    array_to_merge.push_back(channels[80]);

    //Create a matrix object for the false color image
    Mat False_color;

    //Combine the three chosen bands
    cv::merge(array_to_merge, False_color);

    //Convert opencv matrix object to Qt pixmap
    QPixmap pix_false = ASM::cvMatToQPixmap(False_color);

    //Scaling with width and height of label
    int w = ui->LB_falseColor->width();
    int h = ui->LB_falseColor->height();

    //Display false color image in label
    ui->LB_falseColor->setPixmap(pix_false.scaled(w, h, Qt::KeepAspectRatio));*/

    //Grayscale calibrated image using one selected band
    //Mat gray_img_Cal; - Global variable
    //gray_img_Cal =  channels2[40];

    // Apply the colormap:
    //Mat gray_jet_Cal; - Global variable
    //applyColorMap(gray_img_Cal, gray_jet_Cal, COLORMAP_JET);

    if(ui->RB_Rec->isChecked())
    {
        //The current index of the qcombo box
        //int CurrentValue = ui->CB_NBands->currentIndex();

        if (ui->CB_Dark->isChecked() && ui->CB_White->isChecked() )
        {
            //Display the calibrated image in q label
             //Calibrated Image
             //Split Channels
             /*vector<Mat> channels2(Image_Calibrate_8.channels());
             split(Image_Calibrate_8, channels2);

             //Grayscale calibrated image using one selected band
             //Mat gray_img_Cal; - Global variable
             gray_img_Cal =  channels2[CurrentValue];

             // Apply the colormap:
             //Mat gray_jet_Cal; - Global variable
             applyColorMap(gray_img_Cal, gray_jet_Cal, COLORMAP_JET);*/

            //Formulated function
            namedWindow("Select and Save Groups", WINDOW_FREERATIO);
            imshow("Select and Save Groups", gray_jet_Cal);

            if(ui->RB_All_Spectra->isChecked())
            {
                //setting the mouse callback for selecting the region with mouse
                setMouseCallback("Select and Save Groups", mouse_call_Cal_all);

            }else if(ui->RB_Ave_Spectra->isChecked())
            {
                //setting the mouse callback for selecting the region with mouse
                setMouseCallback("Select and Save Groups", mouse_call_Cal_ave);

            }
        }else
        {
            //Split Channels
            /*vector<Mat> channels(image2.channels());
            split(image2, channels);

            //Grayscale image using one selected band
            Mat gray_img =  channels[CurrentValue];

            // Apply the colormap:
            //Mat gray_jet; - Global variable
            applyColorMap(gray_img, gray_jet, COLORMAP_JET);*/

            //Formulated function
            namedWindow("Select and Save Groups", WINDOW_FREERATIO);
            imshow("Select and Save Groups", gray_jet);

            if(ui->RB_All_Spectra->isChecked())
            {
                //setting the mouse callback for selecting the region with mouse
                setMouseCallback("Select and Save Groups", mouse_call_NonCal_all);

            }else if(ui->RB_Ave_Spectra->isChecked())
            {
                //setting the mouse callback for selecting the region with mouse
                setMouseCallback("Select and Save Groups", mouse_call_NonCal_ave);

            }
        }

    }else
    {
        QMessageBox::information(this, "Attention", "Check the Rectangular data selection tool");

    }




}


void MainWindow::on_actionOpen_HSI_DataCube_triggered()
{
    QString filter_w = "All Files(*.*)";

    //Create string for dynamic path
    QString filename_w = QFileDialog::getOpenFileName(this, "Open file", QDir::homePath(), filter_w);

    QMessageBox::information(this, "Attention", "Select Task and perform data selection");
}



void MainWindow::on_PB_ViewBand_clicked()
{
    //The current index of the qcombo box
    int CurrentValue = ui->CB_NBands->currentIndex();

    if (ui->CB_Dark->isChecked() && ui->CB_White->isChecked() )
    {
        //Display the calibrated image in q label
         //Calibrated Image
         //Split Channels
         vector<Mat> channels2(Image_Calibrate_8.channels());
         split(Image_Calibrate_8, channels2);

         //Grayscale calibrated image using one selected band
         //Mat gray_img_Cal; - Global variable
         gray_img_Cal =  channels2[CurrentValue];

         // Apply the colormap:
         //Mat gray_jet_Cal; - Global variable
         applyColorMap(gray_img_Cal, gray_jet_Cal, COLORMAP_JET);

         //Convert opencv matrix object to Qt pixmap
         QPixmap pix_colormap_cal = ASM::cvMatToQPixmap(gray_jet_Cal);

         //Scaling with width and height of label
         int w_cal = ui->LB_falseColor->width();
         int h_cal = ui->LB_falseColor->height();

         //Display gray colormap (jet) image in label
         ui->LB_falseColor->setPixmap(pix_colormap_cal.scaled(w_cal, h_cal, Qt::KeepAspectRatio));
    }else
    {
        //Split Channels
        vector<Mat> channels(image2.channels());
        split(image2, channels);

        //Grayscale image using one selected band
        Mat gray_img =  channels[CurrentValue];

        // Apply the colormap:
        //Mat gray_jet; - Global variable
        applyColorMap(gray_img, gray_jet, COLORMAP_JET);

        //Convert opencv matrix object to Qt pixmap
        QPixmap pix_colormap = ASM::cvMatToQPixmap(gray_jet);

        //Scaling with width and height of label
        int w = ui->LB_falseColor->width();
        int h = ui->LB_falseColor->height();

        //Display gray colormap (jet) image in label
        ui->LB_falseColor->setPixmap(pix_colormap.scaled(w, h, Qt::KeepAspectRatio));

    }

}

void MainWindow::on_actionAbout_triggered()
{
    ui->stackedWidget->setCurrentIndex(3);
}
