#pragma once

#include <QImage>
#include "EPieceType.h"

struct ImagesCache
{
public:
	ImagesCache()
		: m_blackPiece(QImage(QString::fromStdString(":/pieces/black")))
		, m_whitePiece(QImage(QString::fromStdString(":/pieces/white")))
		, m_redPiece(QImage(QString::fromStdString(":/pieces/red")))
	{
	}

	const QImage& GetImage(EPieceType type)
	{
		switch (type)
		{
		case EPieceType::Black:
			return m_blackPiece;
		case EPieceType::White:
			return m_whitePiece;
		case EPieceType::Red:
			return m_redPiece;
		case EPieceType::None:
			break;
		default:
			break;
		}

		return m_whitePiece;
	}

private:
	QImage m_blackPiece;
	QImage m_whitePiece;
	QImage m_redPiece;
};