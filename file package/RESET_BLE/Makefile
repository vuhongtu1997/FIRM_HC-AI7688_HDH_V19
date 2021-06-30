include $(TOPDIR)/rules.mk

PKG_NAME:=RESET_BLE
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/RESET_BLE
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=RESET_BLE
	DEPENDS:= +libstdcpp
endef

define Package/RESET_BLE/description
 If you can't figure out what this program does,
 you're probably brain-dead and need immediate
 medical attention.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/RESET_BLE/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/RESET_BLE $(1)/bin/
endef

$(eval $(call BuildPackage,RESET_BLE))

