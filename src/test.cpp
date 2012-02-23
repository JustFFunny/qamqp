#include "test.h"
#include <QTime>
#include <QFile>

Test::Test()
{
	QUrl con(QString("amqp://guest:guest@localhost:5672/"));
	client_ = new QAMQP::Client(this);
	client_->open(con);
	exchange_ =  client_->createExchange("test.test2");
	exchange_->declare("fanout");

	queue_ = client_->createQueue("test.my_queue", exchange_->channelNumber());
	queue_->declare();

	queue2_ = client_->createQueue("test.my_queue2");
	queue2_->declare();

	exchange_->bind(queue_);
	exchange_->bind(queue2_);

	connect(queue_, SIGNAL(declared()), this, SLOT(declared()));
	connect(queue_, SIGNAL(messageRecieved()), this, SLOT(newMessage()));
	
	connect(queue2_, SIGNAL(messageRecieved()), this, SLOT(newMessage()));
}

Test::~Test()
{

}

void Test::declared()
{
	qDebug("\t-= Ready =-");
	//queue_->purge();
	QFile f("D:/geoip.eap");
	f.open(QIODevice::ReadOnly);
	exchange_->publish(f.readAll(), exchange_->name(), "image/jpg");
	//queue_->remove(true, false, false);
	queue_->setQOS(0,10);
	queue_->setConsumerTag("qamqp-consumer");
	queue_->consume(QAMQP::Queue::coNoAck);

	queue2_->setQOS(0,10);
	queue2_->setConsumerTag("qamqp-consumer2");
	queue2_->consume(QAMQP::Queue::coNoAck);
	//exchange_->remove(false, false);
}

void Test::newMessage()
{
	QAMQP::Queue * q = qobject_cast<QAMQP::Queue *>(sender());
	while (q->hasMessage())
	{
		QAMQP::MessagePtr message = q->getMessage();
		qDebug("+ RECEIVE MESSAGE");
		qDebug("| Exchange-name: %s", qPrintable(message->exchangeName));
		qDebug("| Routing-key: %s", qPrintable(message->routeKey));
		qDebug("| Content-type: %s", qPrintable(message->property[QAMQP::Frame::Content::cpContentType].toString()));

	}

}
