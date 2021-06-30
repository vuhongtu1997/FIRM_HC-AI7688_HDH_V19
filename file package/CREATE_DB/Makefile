include $(TOPDIR)/rules.mk

PKG_NAME:=CREATE_DB
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/CREATE_DB
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=CREATE_DB
	DEPENDS:= +libstdcpp +libsqlite3 +libjson-c
endef

define Package/CREATE_DB/description
 If you can't figure out what this program does,
 you're probably brain-dead and need immediate
 medical attention.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/CREATE_DB/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/CREATE_DB $(1)/bin/
endef

$(eval $(call BuildPackage,CREATE_DB))

