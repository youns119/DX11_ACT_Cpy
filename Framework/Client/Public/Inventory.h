#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Item_Trash.h"

BEGIN(Client)
class CInventory final : public CBase
{
	using super = CBase;

public:
	enum INVEN_TYPE
	{
		INVEN_TRASH,
		INVEN_SHOP,
		INVEN_QUEST,
		INVEN_END,
	};

private:
	CInventory(INVEN_TYPE eInvenType);
	virtual ~CInventory() = default;

public:
	void Add(CItem_Trash* pItem);
	void Add(CItem_Trash* pItem, _uint iAddCount);
	void Add(_uint iTextureNum, _int iAddCount);

	void Delete(CItem_Trash* pItem);
	void Delete(CItem_Trash* pItem, _int iDeleteCount);
	void Delete(_uint iTextureNum, _int iDeleteCount);

	INVEN_TYPE Get_InvenType() { return m_eInvenType; }
	_int Get_Size() { return (_int)m_mapItem.size(); }

	pair<_uint, _int> Get_ItemData(_int iIndex);
	_int				Get_ItemCount(_uint iTextureNum);
private:
	INVEN_TYPE m_eInvenType = {};

	// 텍스쳐 번호, 개수
	map<_uint, _int> m_mapItem;

public:
	static CInventory* Create(INVEN_TYPE eInvenType);
	virtual void Free() override;
};
END
