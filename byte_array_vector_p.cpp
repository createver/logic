#include "byte_array_vector_p.h"

namespace Datalink
{
    ByteArray::ByteArray ()
        : QByteArray()
        , m_useful_size( 0 )
        , m_uuid( QUuid() )
        , m_complete_flag( false )
        , m_dt_last_receive_message()
        , m_count_packet( 0 )
    {
        static int max_size = 200 * 1024;
        resize( max_size );
    }

    ByteArray::~ByteArray ()
    {
    }

    void ByteArray::setSize ( int size_useful )
    {
        //Установим размер буфера не меньше текущего
        //Если размер требуемой памяти больше, чем выделено, то выделим требуемый размер
        if ( size_useful > size() )
        {
            resize( size_useful );
            //qDebug()<<"ResizeReceiveBuffer"<<size_useful;
        }
        //Укажем полезный размер
        m_useful_size = size_useful;
    }

    int ByteArray::usefulSize () const
    {
        return m_useful_size;
    }

    void ByteArray::setUuid ( const QUuid & uuid )
    {
        m_uuid = uuid;
    }

    QUuid ByteArray::currentUuid () const
    {
        return m_uuid;
    }

    bool ByteArray::complete () const
    {
        return m_complete_flag;
    }

    void ByteArray::setCompleteFlag ( bool flag )
    {
        m_complete_flag = flag;
    }

    DateTimeLL ByteArray::lastReceiveMessage () const
    {
        return m_dt_last_receive_message;
    }

    void ByteArray::resetLastReceiveMessage ()
    {
        m_dt_last_receive_message = DateTimeLL::currentDateTime();
    }

    bool ByteArray::olderThan ( qint64 msec ) const
    {
        return m_dt_last_receive_message.msecsTo( DateTimeLL::currentDateTime() ) > msec;
    }

    void ByteArray::resetPacketCounter ()
    {
        m_count_packet = 0;
    }

    void ByteArray::increasePacketCounter ()
    {
        m_count_packet++;
    }

    bool ByteArray::fullPacketReceive ( int n_count )
    {
        return n_count == m_count_packet;
    }


    ////////////////////////////////////////////////////////////////////////////
    /// ByteArrayVector

    ByteArrayVector::ByteArrayVector ( int size )
        : QVector < ByteArray >( size )
        //, sem_stream_process( 1 )
    {
	};

	ByteArrayVector::~ByteArrayVector ( )
    {
    }

    void ByteArrayVector::setStartReceive ( const UdpStreamHeader & header )
    {
        //Попытка зафиксировать ячейку для дальнейшего приема и укладки сообщений
        int index = getFreeCell( header.key_message );

        if ( index < 0 )return;

        Iterator iter = this->begin();
        iter += index;
        if ( iter != this->end() )
        {
            ByteArray & ba4rcv = *iter;
            ba4rcv.setSize( header.all_size );
            ba4rcv.setUuid( header.key_message );
            ba4rcv.resetPacketCounter();
            ba4rcv.resetLastReceiveMessage();
        }
    }

    void ByteArrayVector::saveUsefulMessage ( const UdpStreamHeader & header
                                            , const QByteArray & ba_useful )
    {
        for ( iterator iter = begin(); iter != end(); ++iter )
        {
            ByteArray & ba4rcv = *iter;
            if ( ba4rcv.currentUuid() == header.key_message )
            {
                ba4rcv.replace( header.index_udp_message
                              , ba_useful.size()
                              , ba_useful );
                ba4rcv.increasePacketCounter();
                ba4rcv.resetLastReceiveMessage();
            }
        }
    }

    void ByteArrayVector::completeReceive ( const UdpStreamHeader & header )
    {
        for ( iterator iter = begin(); iter != end(); ++iter )
        {
            ByteArray & ba4rcv = *iter;
            if ( ba4rcv.currentUuid() == header.key_message )
            {
                ba4rcv.setCompleteFlag( ba4rcv.fullPacketReceive( header.count_message ) );
                ba4rcv.resetLastReceiveMessage();
                break;
            }
        }
    }

    QByteArrays ByteArrayVector::completeBuffers ()
    {
        QByteArrays complete_buffers( completeCount() );
        int index = 0;
        for ( iterator iter = begin(); iter != end(); ++iter )
        {
            ByteArray & ba4rcv = *iter;
            if ( ba4rcv.complete() )
            {
                complete_buffers.replace(
                                    index, ba4rcv.left( ba4rcv.usefulSize() ) );
                //Очистим ячейку
                ba4rcv.setUuid( QUuid() );
                ba4rcv.setSize( 0 );
                ba4rcv.setCompleteFlag( false );
                ba4rcv.resetPacketCounter();
                index++;
            }
        }
        return complete_buffers;
    }

    int ByteArrayVector::completeCount () const
    {
        int complete_count = 0;
        for ( ByteArrayVector::ConstIterator iter = this->constBegin();
                                             iter != this->constEnd();
                                             ++iter )
        {
            const ByteArray & ba = *iter;
            if ( ba.complete() )
            {
                complete_count++;
            }
        }
        return complete_count;
    }

    int ByteArrayVector::getFreeCell ( const QUuid & uuid_p ) const
    {
        //Поиск с последнего элемента
        int index = this->count() - 1;

        while ( index >= 0 )
        {
            const ByteArray & ba = this->at( index );

            //Определение свободной ячейки
            if ( ba.currentUuid() == uuid_p //Если есть такая запись
              || ba.currentUuid().isNull()//Если в ячейке не заполнен ключ
              || ba.olderThan( 1 ) )//Не было обновления в течении 1 мс
            {
                break;
            }
			index--;
        }

        return index;
    }
};



