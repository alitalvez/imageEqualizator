#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    myImg = new carregaImage; //Create Image module
    image = &myImg->imageQ; //Copy of image (QImage)

    imageCopy = &myImg->imageQOriginal; //Copy of original image (QImage)

    imageL = myImg->image; //Image in QLabel

    grayScale = myImg->grayScale;

    sizeImage = 0;

    ui->btValidacao->hide(); //Hide Validation button
    ui->btEqualizar->hide(); //Hide Equalizar button
    ui->btSalvar->hide();
    ui->btMakeHist->hide();

    imageL->adjustSize();

    ui->scrollArea->setWidget(imageL); //Set Image on the box

    ui->customPlot->xAxis->setLabel("Tom"); //Hist
    ui->customPlot->yAxis->setLabel("Quantidade"); //Hist
    ui->customPlot->xAxis->setRange(0, 255); //Hist
    ui->customPlot->yAxis->setRange(0, 1000); //Hist
    ui->customPlot->QCustomPlot::setInteractions(QCP::iRangeZoom); //Hist
    ui->customPlot->setInteraction(QCP::iRangeDrag, true); //Hist

    average = &(myImg->average);

    isInside = false;

    connect(ui->btLoad, SIGNAL(released()), this, SLOT(carregar())); //Load image

    connect(ui->slider, SIGNAL(valueChanged(int)), this, SLOT(applySets(int))); // Bright change

    connect(ui->btMakeHist, SIGNAL(released()), this, SLOT(makeHist())); //Hist

    connect(ui->btValidacao, SIGNAL(released()), this, SLOT(validacao())); //Validation

    connect(ui->btEqualizar, SIGNAL(released()), myImg, SLOT(equalizar()));

    connect(ui->btSalvar, SIGNAL(released()), this, SLOT(salvar()));

}

MainWindow::~MainWindow()
{
    delete myImg;
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *clicked)
{
    isInside = false;
    inPosX = clicked->pos().x();
    inPosY = clicked->pos().y();
    if((inPosX > 22) && (inPosX < 293) && (inPosY > 40) && (inPosY < 240))
        isInside = true;
}

void MainWindow::mouseMoveEvent(QMouseEvent *draged)
{
    if(isInside)
    {
        int x, y;
        x = draged->pos().x();
        y = draged->pos().y();
        if((x > 22) && (x < 293))
        {
            y -= inPosY;
            myImg->aplicarBrilho(&y);
            imageL->setPixmap(QPixmap::fromImage(*image));
        }
    }
}

void MainWindow::carregar()
{
    myImg->carregar();
    if(myImg->isImage)
    {
        ui->btEqualizar->show();
        ui->btSalvar->show();
        ui->btMakeHist->show();
        ui->slider->setSliderPosition(0);
        sizeImage = myImg->sizeImage;
    }
}

void MainWindow::salvar()
{
    QString local = QFileDialog::getSaveFileName(this, tr("Escolha local e nome"), QDir::currentPath());
    local = local + ".png";
    image->save(local);
    QMessageBox::information(this, tr("Imagem Salva"), tr("Imagem salva com sucesso"));
}

void MainWindow::applySets(int luz) //Luz is received by QSlider
{
    if(myImg->isImage)
        myImg->aplicarBrilho(&luz);
    //image will be changed and imageCopy will be the INPUT of a original image
    imageL->setPixmap(QPixmap::fromImage(*image)); //Change imageL to display new image
}

void MainWindow::makeHist()
{ //Here i will get the hist and make it a vector, then it will be displayed
    if(myImg->isImage == true)
    {
        QVector<double> x(256), y(256); //My vector
        myImg->scale->cleanScale(grayScale, average);
        myImg->scale->getScale(grayScale, average, image);
        totalGray = 0;

        for(int i = 0; i < 256; i++)
        {
            totalGray += grayScale[i]; //Sum of elements to validation
            x[i] = i;
            y[i] = grayScale[i];
        }
        //Custom Plot functions to plot the graph
        ui->customPlot->addGraph();
        ui->customPlot->graph(0)->setData(x, y);
        ui->customPlot->replot();
        ui->btValidacao->show();
        ui->lbAverage->setText(tr("Média: %1").arg(*average));
    }
}

void MainWindow::validacao()
{
    if (totalGray == sizeImage)
    {
        QMessageBox::information(this, tr("Validacao"), tr("O total do histograma e de %1 pixels e a imagem tem %2 pixels. O histograma esta correto").arg(totalGray).arg(sizeImage));
        return;
    }
    else
    {
        QMessageBox::information(this, tr("Validacao"), tr("Ops, algo deu errado, a imagem e o histograma nao contem a mesma quantidade de pixels."));
        return;
    }
}

