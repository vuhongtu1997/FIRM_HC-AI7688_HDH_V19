include $(TOPDIR)/rules.mk

PKG_NAME:=RD_SMART
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/RD_SMART
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=RD_SMART
	DEPENDS:= +libcurl +libuci +libmosquitto +libstdcpp +libpthread +libsqlite3
endef

define Package/RD_SMART/description
 If you can't figure out what this program does,
 you're probably brain-dead and need immediate
 medical attention.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/RD_SMART/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/RD_SMART $(1)/bin/
endef

$(eval $(call BuildPackage,RD_SMART))

