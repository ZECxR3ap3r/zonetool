// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		void IMenuDef::init(void* ptr, ZoneMemory* mem)
		{
			this->asset_ = reinterpret_cast<menuDef_t*>(ptr);
			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).menu;
			}
			this->name_ = this->asset_->window.name;
		}

		void IMenuDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).menu;
		}

		void IMenuDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IMenuDef::load_depending(IZone* zone)
		{
			auto* data = this->asset_;
			if (data->window.background)
			{
				zone->add_asset_of_type(material, data->window.background->name);
			}
			if (data->data->soundName)
			{
				zone->add_asset_of_type(sound, data->data->soundName);
			}
			if (data->items)
			{
				for (int i = 0; i < data->itemCount; i++)
				{
					if (data->items[i]->window.background)
					{
						zone->add_asset_of_type(material, data->items[i]->window.background->name);
					}
					if (data->items[i]->focusSound)
					{
						zone->add_asset_of_type(sound, data->items[i]->focusSound->name);
					}
					if (data->items[i]->dataType == 6)
					{
						if (data->items[i]->typeData.listBox->selectIcon)
						{
							zone->add_asset_of_type(material, data->items[i]->typeData.listBox->selectIcon->name);
						}
					}
				}
			}
		}

		std::string IMenuDef::name()
		{
			return this->name_;
		}

		std::int32_t IMenuDef::type()
		{
			return menu;
		}

		void IMenuDef::write_menu_statement(IZone* zone, ZoneBuffer* buf, Statement_s* data)
		{
			auto* dest = buf->write(data);
			if (data->entries)
			{
				buf->align(3);
				auto* destEntries = buf->write(data->entries, data->numEntries);
				for (int i = 0; i < data->numEntries; i++)
				{
					if (data->entries[i].type)
					{
						auto* operand = &data->entries[i].data.operand;
						if (operand->dataType)
						{
							if (operand->dataType != 1)
							{
								if (operand->dataType == 2)
								{
									destEntries[i].data.operand.internals.stringVal.string = 
										buf->write_str(operand->internals.stringVal.string);
								}
								else if(operand->dataType == 3)
								{
									buf->align(3);
									write_menu_statement(zone, buf, operand->internals.function);
									ZoneBuffer::clear_pointer(&destEntries[i].data.operand.internals.function);
								}
							}
						}
					}
				}
				ZoneBuffer::clear_pointer(&dest->entries);
			}
			if (data->supportingData)
			{
				buf->align(3);
				auto* destSupData = buf->write(data->supportingData);
				if (data->supportingData->uifunctions.functions)
				{
					buf->align(3);
					auto** destFuncs = buf->write(data->supportingData->uifunctions.functions, 
						data->supportingData->uifunctions.totalFunctions);
					auto** dataFuncs = data->supportingData->uifunctions.functions;
					for (int i = 0; i < data->supportingData->uifunctions.totalFunctions; i++)
					{
						if (dataFuncs[i])
						{
							buf->align(3);
							write_menu_statement(zone, buf, dataFuncs[i]);
							ZoneBuffer::clear_pointer(&destFuncs[i]);
						}
					}
					ZoneBuffer::clear_pointer(&destSupData->uifunctions.functions);
				}
				if (data->supportingData->staticDvarList.staticDvars)
				{
					buf->align(3);
					auto** destStaticDvars = buf->write(data->supportingData->staticDvarList.staticDvars, 
						data->supportingData->staticDvarList.numStaticDvars);
					auto** dataStaticDvars = data->supportingData->staticDvarList.staticDvars;
					for (int i = 0; i < data->supportingData->staticDvarList.numStaticDvars; i++)
					{
						if (dataStaticDvars[i])
						{
							buf->align(3);
							auto* destVar = buf->write(dataStaticDvars[i]);
							if (dataStaticDvars[i]->dvarName)
							{
								destVar->dvarName = buf->write_str(dataStaticDvars[i]->dvarName);
							}
							ZoneBuffer::clear_pointer(&destStaticDvars[i]);
						}
					}
					ZoneBuffer::clear_pointer(&destSupData->staticDvarList.staticDvars);
				}
				if (data->supportingData->uiStrings.strings)
				{
					buf->align(3);
					auto* destStrings = buf->write(data->supportingData->uiStrings.strings, 
						data->supportingData->uiStrings.totalStrings);
					for (int i = 0; i < data->supportingData->uiStrings.totalStrings; i++)
					{
						if (data->supportingData->uiStrings.strings[i])
						{
							destStrings[i] = buf->write_str(data->supportingData->uiStrings.strings[i]);
						}
					}
					ZoneBuffer::clear_pointer(&destSupData->uiStrings.strings);
				}

				ZoneBuffer::clear_pointer(&dest->supportingData);
			}
		}

		void IMenuDef::write_menu_eventhandlerset(IZone* zone, ZoneBuffer* buf, MenuEventHandlerSet* data)
		{
			auto* dest = buf->write(data);
			if (data->eventHandlers)
			{
				buf->align(3);
				auto* destHandlers = buf->write(data->eventHandlers, data->eventHandlerCount);
				for (int i = 0; i < data->eventHandlerCount; i++)
				{
					if (data->eventHandlers[i])
					{
						buf->align(3);
						auto* destHandler = buf->write(data->eventHandlers[i]);
						auto* dataHandler = data->eventHandlers[i];
						switch (dataHandler->eventType)
						{
						case 0:
							destHandler->eventData.unconditionalScript = buf->write_str(dataHandler->eventData.unconditionalScript);
							break;
						case 1:
							if (dataHandler->eventData.conditionalScript)
							{
								buf->align(3);
								auto* destScript = buf->write(dataHandler->eventData.conditionalScript);
								if (dataHandler->eventData.conditionalScript->eventExpression)
								{
									buf->align(3);
									write_menu_statement(zone, buf, dataHandler->eventData.conditionalScript->eventExpression);
									ZoneBuffer::clear_pointer(&destScript->eventExpression);
								}
								if (dataHandler->eventData.conditionalScript->eventHandlerSet)
								{
									buf->align(3);
									write_menu_eventhandlerset(zone, buf, dataHandler->eventData.conditionalScript->eventHandlerSet);
									ZoneBuffer::clear_pointer(&destScript->eventHandlerSet);
								}
								ZoneBuffer::clear_pointer(&destHandler->eventData.conditionalScript);
							}
							break;
						case 2:
							if (dataHandler->eventData.elseScript)
							{
								buf->align(3);
								write_menu_eventhandlerset(zone, buf, dataHandler->eventData.elseScript);
								ZoneBuffer::clear_pointer(&destHandler->eventData.elseScript);
							}
							break;
						case 3:
						case 4:
						case 5:
						case 6:
							if (dataHandler->eventData.setLocalVarData)
							{
								buf->align(3);
								auto* destLVD = buf->write(dataHandler->eventData.setLocalVarData);
								if (dataHandler->eventData.setLocalVarData->localVarName)
								{
									destLVD->localVarName = buf->write_str(dataHandler->eventData.setLocalVarData->localVarName);
								}
								if (dataHandler->eventData.setLocalVarData->expression)
								{
									buf->align(3);
									write_menu_statement(zone, buf, dataHandler->eventData.setLocalVarData->expression);
									ZoneBuffer::clear_pointer(&destLVD->expression);
								}

								ZoneBuffer::clear_pointer(&destHandler->eventData.setLocalVarData);
							}
							break;
						}
						ZoneBuffer::clear_pointer(&destHandlers[i]);
					}
				}
				ZoneBuffer::clear_pointer(&dest->eventHandlers);
			}
		}

		void IMenuDef::write_menu_itemkeyhandler(IZone* zone, ZoneBuffer* buf, ItemKeyHandler* data)
		{
			auto* dest = buf->write(data);
			if (data->action)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->action);
				ZoneBuffer::clear_pointer(&dest->action);
			}
			if (!data->next)
				return;
			buf->align(3);
			write_menu_itemkeyhandler(zone, buf, data->next);
			ZoneBuffer::clear_pointer(&dest->next);
		}

		void IMenuDef::write_menu_data(IZone* zone, ZoneBuffer* buf, menuData_t* data)
		{
			auto* dest = buf->write(data);
			if (data->expressionData)
			{
				buf->align(3);
				auto* destExpSupData = buf->write(data->expressionData);
				if (data->expressionData->uifunctions.functions)
				{
					buf->align(3);
					auto** destExpData = buf->write(
						data->expressionData->uifunctions.functions, 
						data->expressionData->uifunctions.totalFunctions);
					for (int i = 0; i < data->expressionData->uifunctions.totalFunctions; i++)
					{
						if (data->expressionData->uifunctions.functions[i])
						{
							buf->align(3);
							write_menu_statement(zone, buf, data->expressionData->uifunctions.functions[i]);
							ZoneBuffer::clear_pointer(&destExpData[i]);
						}
					}

					ZoneBuffer::clear_pointer(&destExpSupData->uifunctions.functions);
				}
				ZoneBuffer::clear_pointer(&dest->expressionData);
			}
			if (data->onOpen)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->onOpen);
				ZoneBuffer::clear_pointer(&dest->onOpen);
			}
			if (data->onClose)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->onClose);
				ZoneBuffer::clear_pointer(&dest->onClose);
			}
			if (data->onCloseRequest)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->onCloseRequest);
				ZoneBuffer::clear_pointer(&dest->onCloseRequest);
			}
			if (data->onESC)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->onESC);
				ZoneBuffer::clear_pointer(&dest->onESC);
			}
			if (data->onFocusDueToClose)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->onFocusDueToClose);
				ZoneBuffer::clear_pointer(&dest->onFocusDueToClose);
			}
			if (data->onKey)
			{
				buf->align(3);
				write_menu_itemkeyhandler(zone, buf, data->onKey);
				ZoneBuffer::clear_pointer(&dest->onKey);
			}
			if (data->visibleExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->visibleExp);
				ZoneBuffer::clear_pointer(&dest->visibleExp);
			}
			if (data->allowedBinding)
			{
				dest->allowedBinding = buf->write_str(data->allowedBinding);
			}
			if (data->soundName)
			{
				dest->soundName = buf->write_str(data->soundName);
			}
			if (data->rectXExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->rectXExp);
				ZoneBuffer::clear_pointer(&dest->rectXExp);
			}
			if (data->rectYExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->rectYExp);
				ZoneBuffer::clear_pointer(&dest->rectYExp);
			}
			if (data->rectWExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->rectWExp);
				ZoneBuffer::clear_pointer(&dest->rectWExp);
			}
			if (data->rectHExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->rectHExp);
				ZoneBuffer::clear_pointer(&dest->rectHExp);
			}
			if (data->openSoundExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->openSoundExp);
				ZoneBuffer::clear_pointer(&dest->openSoundExp);
			}
			if (data->closeSoundExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->closeSoundExp);
				ZoneBuffer::clear_pointer(&dest->closeSoundExp);
			}
			if (data->soundLoopExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->soundLoopExp);
				ZoneBuffer::clear_pointer(&dest->soundLoopExp);
			}
		}

		void IMenuDef::write_menu_window(IZone* zone, ZoneBuffer* buf, windowDef_t* data, windowDef_t* dest)
		{
			if (data->name)
			{
				dest->name = buf->write_str(data->name);
			}
			if (data->group)
			{
				dest->group = buf->write_str(data->group);
			}
			if (data->background)
			{
				dest->background = reinterpret_cast<Material*>(zone->get_asset_pointer(material, data->background->name));
			}
		}

		void IMenuDef::write_menu_defdata(IZone* zone, ZoneBuffer* buf, itemDefData_t* data, int type)
		{
			switch (type)
			{
			case 6:
				if (data->listBox)
				{
					auto* dest = buf->write(data->listBox);
					if (data->listBox->onDoubleClick)
					{
						buf->align(3);
						write_menu_eventhandlerset(zone, buf, data->listBox->onDoubleClick);
						ZoneBuffer::clear_pointer(&dest->onDoubleClick);
					}
					if (data->listBox->selectIcon)
					{
						dest->selectIcon = reinterpret_cast<Material*>(
							zone->get_asset_pointer(material, data->listBox->selectIcon->name));
					}
					if (data->listBox->elementHeightExp)
					{
						buf->align(3);
						write_menu_statement(zone, buf, data->listBox->elementHeightExp);
						ZoneBuffer::clear_pointer(&dest->elementHeightExp);
					}
				}
				break;
			case 4:
			case 9:
			case 16:
			case 18:
			case 11:
			case 14:
			case 10:
			case 0:
			case 17:
			case 22:
			case 23:
				if (data->editField)
				{
					/*auto* dest = */buf->write(data->editField);
				}
				break;
			case 12:
				if (data->multi)
				{
					auto* dest = buf->write(data->multi);
					for (int i = 0; i < 32; i++)
					{
						if (data->multi->dvarList[i])
						{
							dest->dvarList[i] = buf->write_str(data->multi->dvarList[i]);
						}
					}
					for (int i = 0; i < 32; i++)
					{
						if (data->multi->dvarStr[i])
						{
							dest->dvarStr[i] = buf->write_str(data->multi->dvarStr[i]);
						}
					}
				}
				break;
			case 13:
				if (data->enumDvarName)
				{
					buf->write_str(data->enumDvarName);
				}
				break;
			case 20:
				if (data->ticker)
				{
					/*auto* dest = */buf->write(data->ticker);
				}
				break;
			case 21:
				if (data->scroll)
				{
					/*auto* dest = */buf->write(data->scroll);
				}
				break;
			}
		}

		void IMenuDef::write_menu_floatexpressions(IZone* zone, ZoneBuffer* buf, ItemFloatExpression* data, int count)
		{
			auto* dest = buf->write(data, count);
			for (int i = 0; i < count; i++)
			{
				if (data[i].expression)
				{
					buf->align(3);
					write_menu_statement(zone, buf, data[i].expression);
					ZoneBuffer::clear_pointer(&dest[i].expression);
				}
			}
		}

		void IMenuDef::write_menu_item(IZone* zone, ZoneBuffer* buf, itemDef_t* data)
		{
			auto* dest = buf->write(data);
			write_menu_window(zone, buf, &data->window, &dest->window);
			if (data->text)
			{
				dest->text = buf->write_str(data->text);
			}
			if (data->mouseEnterText)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->mouseEnterText);
				ZoneBuffer::clear_pointer(&dest->mouseEnterText);
			}
			if (data->mouseExitText)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->mouseExitText);
				ZoneBuffer::clear_pointer(&dest->mouseExitText);
			}
			if (data->mouseEnter)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->mouseEnter);
				ZoneBuffer::clear_pointer(&dest->mouseEnter);
			}
			if (data->mouseExit)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->mouseExit);
				ZoneBuffer::clear_pointer(&dest->mouseExit);
			}
			if (data->action)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->action);
				ZoneBuffer::clear_pointer(&dest->action);
			}
			if (data->accept)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->accept);
				ZoneBuffer::clear_pointer(&dest->accept);
			}
			if (data->onFocus)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->onFocus);
				ZoneBuffer::clear_pointer(&dest->onFocus);
			}
			if (data->hasFocus)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->hasFocus);
				ZoneBuffer::clear_pointer(&dest->hasFocus);
			}
			if (data->leaveFocus)
			{
				buf->align(3);
				write_menu_eventhandlerset(zone, buf, data->leaveFocus);
				ZoneBuffer::clear_pointer(&dest->leaveFocus);
			}
			if (data->dvar)
			{
				dest->dvar = buf->write_str(data->dvar);
			}
			if (data->dvarTest)
			{
				dest->dvarTest = buf->write_str(data->dvarTest);
			}
			if (data->onKey)
			{
				buf->align(3);
				write_menu_itemkeyhandler(zone, buf, data->onKey);
				ZoneBuffer::clear_pointer(&dest->onKey);
			}
			if (data->enableDvar)
			{
				dest->enableDvar = buf->write_str(data->enableDvar);
			}
			if (data->localVar)
			{
				dest->localVar = buf->write_str(data->localVar);
			}
			if (data->focusSound)
			{
				dest->focusSound = reinterpret_cast<snd_alias_list_t*>(zone->get_asset_pointer(sound, data->focusSound->name));
			}
			if (data->typeData.data)
			{
				buf->align(3);
				write_menu_defdata(zone, buf, &data->typeData, data->type);
				ZoneBuffer::clear_pointer(&dest->typeData.data);
			}
			if (data->floatExpressions)
			{
				buf->align(3);
				write_menu_floatexpressions(zone, buf, data->floatExpressions, data->floatExpressionCount);
				ZoneBuffer::clear_pointer(&dest->floatExpressions);
			}
			if (data->visibleExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->visibleExp);
				ZoneBuffer::clear_pointer(&dest->visibleExp);
			}
			if (data->disabledExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->disabledExp);
				ZoneBuffer::clear_pointer(&dest->disabledExp);
			}
			if (data->textExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->textExp);
				ZoneBuffer::clear_pointer(&dest->textExp);
			}
			if (data->materialExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->materialExp);
				ZoneBuffer::clear_pointer(&dest->materialExp);
			}
			if (data->textAlignYExp)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data->textAlignYExp);
				ZoneBuffer::clear_pointer(&dest->textAlignYExp);
			}
		}

		void IMenuDef::write(IZone* zone, ZoneBuffer* buf)
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			// menudata
			if (data->data)
			{
				buf->align(3);
				write_menu_data(zone, buf, data->data);
				ZoneBuffer::clear_pointer(&dest->data);
			}

			write_menu_window(zone, buf, &data->window, &dest->window);

			// items
			if (data->items)
			{
				buf->align(3);
				auto* destItems = buf->write(data->items, data->itemCount);
				for (int i = 0; i < data->itemCount; i++)
				{
					if (data->items[i])
					{
						buf->align(3);
						write_menu_item(zone, buf, data->items[i]);
						ZoneBuffer::clear_pointer(&destItems[i]);
					}
				}
				ZoneBuffer::clear_pointer(&dest->items);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IMenuDef::dump(menuDef_t* asset)
		{
		}
	}
}
