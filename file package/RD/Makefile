include $(TOPDIR)/rules.mk

PKG_NAME:=RD
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/RD
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=RD
	DEPENDS:= +libcurl +libuci +libmosquitto-ssl +libmosquitto +libjson-c +libstdcpp
endef

define Package/RD/description
 If you can't figure out what this program does,
 you're probably brain-dead and need immediate
 medical attention.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/RD/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/RD $(1)/bin/
endef

$(eval $(call BuildPackage,RD))

