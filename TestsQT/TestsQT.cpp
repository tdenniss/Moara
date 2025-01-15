#include <QtTest>
#include <QProcess>

class TestsQT : public QObject
{
	Q_OBJECT

private slots:
	void initTestCase() {
		qDebug("Called before the first test function is executed.");
	}

	void init() {
		qDebug("Called before each test function is executed.");
	}

	void myTest() {
		
	}

	void cleanup() {
		qDebug("Called after every test function.");
	}

	void cleanupTestCase() {
		qDebug("Called after the last test function was executed.");
	}
};

QTEST_MAIN(TestsQT)
#include "TestsQT.moc"
