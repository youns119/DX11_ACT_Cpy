#include "pch.h"
#include "Inventory.h"

#include "GameInstance.h"

CInventory::CInventory(INVEN_TYPE eInvenType)
	: super()
{
	m_eInvenType = eInvenType;
}

void CInventory::Add(CItem_Trash* pItem)
{
	_uint iTextureNum = pItem->Get_ItemNum();
	m_mapItem[iTextureNum]++;
}

void CInventory::Add(CItem_Trash* pItem, _uint iAddCount)
{
	_uint iTextureNum = pItem->Get_ItemNum();
	m_mapItem[iTextureNum] += iAddCount;
}

void CInventory::Add(_uint iTextureNum, _int iAddCount)
{
	m_mapItem[iTextureNum] += iAddCount;
}

void CInventory::Delete(CItem_Trash* pItem)
{
	_uint iTextureNum = pItem->Get_ItemNum();

	if (m_mapItem[iTextureNum] > 0)
	{
		m_mapItem[iTextureNum]--;

		if (m_mapItem[iTextureNum] <= 0)
			m_mapItem.erase(iTextureNum);
	}
}

void CInventory::Delete(CItem_Trash* pItem, _int iDeleteCount)
{
	_uint iTextureNum = pItem->Get_ItemNum();

	if (m_mapItem[iTextureNum] >= iDeleteCount)
	{
		m_mapItem[iTextureNum] -= iDeleteCount;

		if (m_mapItem[iTextureNum] <= 0)
			m_mapItem.erase(iTextureNum);
	}
}

void CInventory::Delete(_uint iTextureNum, _int iDeleteCount)
{
	if (m_mapItem[iTextureNum] >= iDeleteCount)
	{
		m_mapItem[iTextureNum] -= iDeleteCount;

		if (m_mapItem[iTextureNum] <= 0)
			m_mapItem.erase(iTextureNum);
	}
}

pair<_uint, _int> CInventory::Get_ItemData(_int iIndex)
{
	if (m_mapItem.size() <= iIndex)
		return {0, 0};

	auto iter = next(m_mapItem.begin(), iIndex);

	return { iter->first, iter->second };
}

_int CInventory::Get_ItemCount(_uint iTextureNum)
{
	_int iCount = m_mapItem[iTextureNum];
	if (iCount <= 0)
		m_mapItem.erase(iTextureNum);

	return iCount;
}

CInventory* CInventory::Create(INVEN_TYPE eInvenType)
{
	return new CInventory(eInvenType);
}

void CInventory::Free()
{
	super::Free();
}
