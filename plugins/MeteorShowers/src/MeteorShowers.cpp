/*
 * Stellarium: Meteor Showers Plug-in
 * Copyright (C) 2013-2015 Marcos Cardinot
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#include <QtMath>

#include "MeteorShowers.hpp"
#include "StelApp.hpp"
#include "StelModuleMgr.hpp"
#include "StelObjectMgr.hpp"
#include "StelTextureMgr.hpp"
#include "StelUtils.hpp"

MeteorShowers::MeteorShowers(MeteorShowersMgr* mgr)
	: m_mgr(mgr)
{
	GETSTELMODULE(StelObjectMgr)->registerStelObjectMgr(this);
}

MeteorShowers::~MeteorShowers()
{
	m_meteorShowers.clear();
}

void MeteorShowers::update(double deltaTime)
{
	foreach (const MeteorShowerP& ms, m_meteorShowers)
	{
		ms->update(deltaTime);
	}
}

void MeteorShowers::draw(StelCore* core)
{
	foreach (const MeteorShowerP& ms, m_meteorShowers)
	{
		ms->draw(core);
	}

	if (GETSTELMODULE(StelObjectMgr)->getFlagSelectedObjectPointer())
	{
		drawPointer(core);
	}
}

void MeteorShowers::drawPointer(StelCore* core)
{
	const QList<StelObjectP> newSelected = GETSTELMODULE(StelObjectMgr)->getSelectedObject("MeteorShower");
	if (newSelected.empty())
	{
		return;
	}

	const StelObjectP obj = newSelected[0];
	Vec3d pos = obj->getJ2000EquatorialPos(core);

	// Compute 2D pos and return if outside screen
	Vec3d screenpos;
	StelPainter painter(core->getProjection(StelCore::FrameJ2000));
	if (!painter.getProjector()->project(pos, screenpos))
	{
		return;
	}

	const Vec3f& c(obj->getInfoColor());
	painter.setColor(c[0],c[1],c[2]);
	m_mgr->getPointerTexture()->bind();
	painter.enableTexture2d(true);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Normal transparency mode

	float size = obj->getAngularSize(core) * M_PI / 180. * painter.getProjector()->getPixelPerRadAtCenter();
	size += 20.f + 10.f * qSin(2.f * StelApp::getInstance().getTotalRunTime());

	painter.drawSprite2dMode(screenpos[0]-size/2, screenpos[1]-size/2, 10.f, 90);
	painter.drawSprite2dMode(screenpos[0]-size/2, screenpos[1]+size/2, 10.f, 0);
	painter.drawSprite2dMode(screenpos[0]+size/2, screenpos[1]+size/2, 10.f, -90);
	painter.drawSprite2dMode(screenpos[0]+size/2, screenpos[1]-size/2, 10.f, -180);
	painter.setColor(1, 1, 1, 0);
}

void MeteorShowers::loadMeteorShowers(const QVariantMap& map)
{
	m_meteorShowers.clear();
	foreach(QString msKey, map.keys())
	{
		QVariantMap msData = map.value(msKey).toMap();
		msData["showerID"] = msKey;

		MeteorShowerP ms(new MeteorShower(msData));
		if (ms->getStatus() != MeteorShower::INVALID)
		{
			m_meteorShowers.append(ms);
		}
	}
}

QList<MeteorShowerP> MeteorShowers::searchEvents(QDate dateFrom, QDate dateTo) const
{
	QList<MeteorShowerP> result;
	QDate date;
	foreach(const MeteorShowerP& ms, m_meteorShowers)
	{
		date = dateFrom;
		while(date.operator <=(dateTo))
		{
			ms->updateCurrentData((QDateTime) date);
			if (ms->getStatus() == MeteorShower::ACTIVE_GENERIC
				|| ms->getStatus() == MeteorShower::ACTIVE_REAL)
			{
				result.append(ms);
				break;
			}
			date = date.addDays(1);
		}
	}
	return result;
}

QList<StelObjectP> MeteorShowers::searchAround(const Vec3d& av, double limitFov, const StelCore*) const
{
	QList<StelObjectP> result;
	if (!m_mgr->getEnablePlugin())
	{
		return result;
	}

	Vec3d v(av);
	v.normalize();
	double cosLimFov = qCos(limitFov * M_PI/180.);
	Vec3d equPos;
	foreach(const MeteorShowerP& ms, m_meteorShowers)
	{
		if (ms->enabled())
		{
			equPos = ms->getJ2000EquatorialPos(NULL);
			equPos.normalize();
			if (equPos[0]*v[0] + equPos[1]*v[1] + equPos[2]*v[2] >= cosLimFov)
			{
				result.append(qSharedPointerCast<StelObject>(ms));
			}
		}
	}
	return result;
}

StelObjectP MeteorShowers::searchByName(const QString& englishName) const
{
	if (!m_mgr->getEnablePlugin())
	{
		return NULL;
	}

	foreach(const MeteorShowerP& ms, m_meteorShowers)
	{
		if (ms->enabled())
		{
			bool sameEngName = ms->getEnglishName().toUpper() == englishName.toUpper();
			bool desigIsEngName = ms->getDesignation().toUpper() == englishName.toUpper();
			bool emptyDesig = ms->getDesignation().isEmpty();
			if (sameEngName || (desigIsEngName && !emptyDesig))
			{
				return qSharedPointerCast<StelObject>(ms);
			}
		}
	}
	return NULL;
}

StelObjectP MeteorShowers::searchByNameI18n(const QString& nameI18n) const
{
	if (!m_mgr->getEnablePlugin())
	{
		return NULL;
	}

	foreach(const MeteorShowerP& ms, m_meteorShowers)
	{
		if (ms->enabled())
		{
			if (ms->getNameI18n().toUpper() == nameI18n.toUpper())
			{
				return qSharedPointerCast<StelObject>(ms);
			}
		}
	}
	return NULL;
}

QStringList MeteorShowers::listMatchingObjectsI18n(const QString& objPrefix, int maxNbItem, bool useStartOfWords) const
{
	QStringList result;
	if (!m_mgr->getEnablePlugin() || maxNbItem == 0)
	{
		return result;
	}

	QString sn;
	bool found = false;
	foreach(const MeteorShowerP& ms, m_meteorShowers)
	{
		if (ms->enabled())
		{
			sn = ms->getNameI18n();
			if (useStartOfWords)
			{
				found = sn.toUpper().left(objPrefix.length()) == objPrefix.toUpper();
			}
			else
			{
				found = sn.contains(objPrefix, Qt::CaseInsensitive);
			}

			if (found)
			{
				result.append(sn);
			}
		}
	}

	result.sort();
	if (result.size() > maxNbItem)
	{
		result.erase(result.begin() + maxNbItem, result.end());
	}

	return result;
}

QStringList MeteorShowers::listMatchingObjects(const QString& objPrefix, int maxNbItem, bool useStartOfWords) const
{
	QStringList result;
	if (!m_mgr->getEnablePlugin() || maxNbItem == 0)
	{
		return result;
	}

	QString sn;
	bool found = false;
	foreach(const MeteorShowerP& ms, m_meteorShowers)
	{
		if (ms->enabled())
		{
			sn = ms->getEnglishName();
			if (useStartOfWords)
			{
				found = objPrefix.toUpper()==sn.toUpper().left(objPrefix.length());
			}
			else
			{
				found = sn.contains(objPrefix, Qt::CaseInsensitive);
			}
			if (found)
			{
				result.append(sn);
			}

			sn = ms->getDesignation();
			if (useStartOfWords)
			{
				found = objPrefix.toUpper()==sn.toUpper().left(objPrefix.length());
			}
			else
			{
				found = sn.contains(objPrefix, Qt::CaseInsensitive);
			}
			if (found)
			{
				result.append(sn);
			}
		}
	}

	result.sort();
	if (result.size() > maxNbItem)
	{
		result.erase(result.begin() + maxNbItem, result.end());
	}
	return result;
}

QStringList MeteorShowers::listAllObjects(bool inEnglish) const
{
	QStringList result;
	if (!m_mgr->getEnablePlugin())
	{
		return result;
	}

	if (inEnglish)
	{
		foreach(const MeteorShowerP& ms, m_meteorShowers)
		{
			result.append(ms->getEnglishName());
		}
	}
	else
	{
		foreach(const MeteorShowerP& ms, m_meteorShowers)
		{
			result.append(ms->getNameI18n());
		}
	}
	return result;
}
