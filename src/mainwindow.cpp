#include "mainwindow.hpp"

#include "HttpWalker.hpp"
#include "ui_untitled.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : startEnabled{true}, ui{std::make_unique<Ui::MainWindow>()},
      walker{std::make_unique<HttpWalker>()}, status{std::make_unique<QLabel>(
                                                  this)} {
	ui->setupUi(this);
	statusBar()->addWidget(status.get());
	connect(this, &MainWindow::startGrabbing, walker.get(), &HttpWalker::start);
	connect(this, &MainWindow::stopGrabbing, walker.get(), &HttpWalker::stop);
	connect(walker.get(), &HttpWalker::foundItem, this,
	        [&](QString url, unsigned short code) {
		        std::cout << url.toStdString() << " " << code << std::endl;
		        if (code != 200) {
			        ui->listWidget->addItem(url + " " + QString::number(code));
		        }
	        },
	        Qt::QueuedConnection);
	connect(walker.get(), &HttpWalker::progress, this,
	        [&](size_t current, size_t all) {
		        // std::cout << current << " " << all << std::endl;
		        ui->progressBar->setMaximum(all);
		        ui->progressBar->setValue(current);
		        status->setText(QString::number(current) + "/" +
		                        QString::number(all));
	        },
	        Qt::QueuedConnection);
	connect(walker.get(), &HttpWalker::started, this,
	        [&]() {
		        status->setText("Looking up");
		        ui->listWidget->clear();
		        startEnabled = false;
		        ui->startButton->setText("Stop");
		        ui->urlEdit->setEnabled(false);
	        },
	        Qt::QueuedConnection);
	connect(walker.get(), &HttpWalker::finished, this,
	        [&]() {
		        status->setText("Finished " + status->text());
		        startEnabled = true;
		        ui->startButton->setText("Start");
		        ui->urlEdit->setEnabled(true);
	        },
	        Qt::QueuedConnection);
	connect(ui->startButton, &QPushButton::clicked, [=, this] {
		if (startEnabled) {
			std::string url = ui->urlEdit->text().toStdString();
			emit startGrabbing(url);
		} else {
			emit stopGrabbing();
		}
	});
}
MainWindow::~MainWindow() {}
